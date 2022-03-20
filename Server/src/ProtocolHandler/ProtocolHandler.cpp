#include "ProtocolHandler.h"

#include "..//Application/Application.h"
#include <iostream>

#include "..//stb_image/stb_image.h"
#include <filesystem>
#include <fstream>

struct XMsgServerUserData {
	std::ptrdiff_t m_nUserId = 0;
	std::ptrdiff_t m_nAccountId = 0;
	bool IsAuth = false;
};

std::vector<XMsgServerUserData> m_vUserData;

XMsg::Server::ProtocolHandler* XMsg::Server::ProtocolHandler::m_pInstance;

XMsg::Server::ProtocolHandler::ProtocolHandler(){
	m_pDataBase = new XMsg::Server::UserBase;
	XMsg::Server::ProtocolHandler::m_pInstance = this;
}

XMsg::Server::ProtocolHandler* XMsg::Server::ProtocolHandler::GetInstanse(){
	return m_pInstance;
}

XMsg::Server::UserBase* XMsg::Server::ProtocolHandler::GetDataBase(){
	return m_pDataBase;
}

void XMsg::Server::ProtocolHandler::SendIcon(const std::string& IconPath, Api* api, std::ptrdiff_t m_nUserId){
	int width = 288, height = 288, nrChannels = 3;
	//unsigned char* Data = stbi_load(IconPath.c_str(), &width, &height, &nrChannels, 0);
	
	//if (Data == NULL) {
	//	api->SendProtoStatus(m_nUserId, false);
	//	stbi_image_free(Data);
	//	return;
	//}

	//int Len = std::filesystem::file_size(IconPath);
	//int Len = width * height * nrChannels + 10000;

	std::ifstream m_Fstream(IconPath, std::ios::binary | std::ios::ate);
	m_Fstream.unsetf(std::ios::skipws);

	char Symbol;
	std::vector<char> m_sStr;

	m_Fstream.seekg(0, std::ios::end);
	int Len = m_Fstream.tellg();
	m_Fstream.seekg(0, std::ios::beg);
	
	m_sStr.resize(Len);

	m_sStr.insert(m_sStr.begin(), std::istream_iterator<char>(m_Fstream), std::istream_iterator<char>());

	std::cout << "file size:" << Len << std::endl;
	
	m_Fstream.close();

	std::cout << "Image len:" << Len << std::endl;
	std::cout << "Image len2:" << m_sStr.size() << std::endl;
	std::cout << "Image width:" << width << std::endl;
	std::cout << "Image height:" << height << std::endl;
	std::cout << "Image nrChannels:" << nrChannels << std::endl;
	std::cout << "Image sizeof(unsigned char):" << sizeof(unsigned char) << std::endl;

	api->SendNum(m_nUserId, width);
	api->SendNum(m_nUserId, height);
	api->SendNum(m_nUserId, nrChannels);
	
	api->SendData(m_nUserId, reinterpret_cast<char*>(m_sStr.data()), Len);

	//stbi_image_free(Data);

	std::cout << "Image :" << IconPath << " Sended!" << std::endl;
}

bool XMsg::Server::ProtocolHandler::HandleProtocol(Api* api, XPlatform::Api::u32 m_nProtoId, std::ptrdiff_t m_nUserId){

	std::cout << "Request: ProtoId{" << m_nProtoId << "}, UserId{" << m_nUserId << "}" << std::endl;

	switch (m_nProtoId){
	case XMSG_API_PROTOCOL_ID_LOGIN: {
			api->SendProtoStatus(m_nUserId, true);
			if (m_vUserData.size() < m_nUserId);
				m_vUserData.resize(m_nUserId + 1); 
			XMsg::Server::UserBaseData BaseUserData;

			// Client Send Protocol AuthStatus
			api->RecvProto(m_nUserId);
			api->SendProtoStatus(m_nUserId, m_vUserData[m_nUserId].IsAuth);

			if (m_vUserData[m_nUserId].IsAuth) {
				api->SendProto(m_nUserId, XMSG_API_PROTOCOL_ID_DISCONNECT);
				std::cout << "authentication attempt provided that the user is already authenticated !" << std::endl;
				return false;
			}

			XPlatform::Api::u32 ProtoId;
			std::vector<char> m_pData = api->RecvData(ProtoId, m_nUserId);
			m_pData.push_back('\0');
			api->SendProtoStatus(m_nUserId, true);
			if (ProtoId == 1) {
				// Login - XMSG_API_DATA_TYPE_ID_LOGIN
				BaseUserData.m_sLogin = m_pData.data();
			}
			else if (ProtoId == 2) {
				// Password - XMSG_API_DATA_TYPE_ID_PASSWORD
				BaseUserData.m_sPassword = m_pData.data();
			}

			m_pData = api->RecvData(ProtoId, m_nUserId);
			api->SendProtoStatus(m_nUserId, true);
			m_pData.push_back('\0');
			if (ProtoId == 1) {
				// Login - XMSG_API_DATA_TYPE_ID_LOGIN
				BaseUserData.m_sLogin = m_pData.data();
			}
			else if (ProtoId == 2) {
				// Password - XMSG_API_DATA_TYPE_ID_PASSWORD
				BaseUserData.m_sPassword = m_pData.data();
			}

			m_vUserData[m_nUserId].IsAuth = m_pDataBase->AuthificateUser(BaseUserData.m_sLogin, BaseUserData.m_sPassword);
			if (m_vUserData[m_nUserId].IsAuth) {
				m_vUserData[m_nUserId].m_nUserId = m_nUserId;
				std::cout << "loggined: " << BaseUserData.m_sLogin << std::endl;
			}

			api->RecvProto(m_nUserId);
			api->SendProtoStatus(m_nUserId, m_vUserData[m_nUserId].IsAuth);

			m_vUserData[m_nUserId].m_nAccountId = m_pDataBase->GetUserId(BaseUserData.m_sLogin);
		return m_vUserData[m_nUserId].IsAuth;
	}
	case XMSG_API_PROTOCOL_ID_GET_FRIENDS_NAMES:
		api->SendProtoStatus(m_nUserId, m_vUserData[m_nUserId].IsAuth);
		if (m_vUserData[m_nUserId].IsAuth) {
			std::vector<std::ptrdiff_t>& m_vFriendsAccountsId = m_pDataBase->GetFriends(m_vUserData[m_nUserId].m_nAccountId);

			api->SendNumLong(m_nUserId, m_vFriendsAccountsId.size());

			for (std::ptrdiff_t m_nAccountId : m_vFriendsAccountsId) {
				std::string AccountLogin = m_pDataBase->GetAccountLogin(m_nAccountId);

				api->SendMsg(m_nUserId, AccountLogin);
			}

		}
		return true;
	case XMSG_API_PROTOCOL_ID_GET_FRIENDS_ICONS: {
		api->SendProtoStatus(m_nUserId, m_vUserData[m_nUserId].IsAuth);
		if (m_vUserData[m_nUserId].IsAuth) {
			std::vector<std::ptrdiff_t>& m_vFriendsAccountsId = m_pDataBase->GetFriends(m_vUserData[m_nUserId].m_nAccountId);

			api->SendNumLong(m_nUserId, m_vFriendsAccountsId.size());

			for (std::ptrdiff_t m_nAccountId : m_vFriendsAccountsId) {
				std::string IconPath = m_pDataBase->GetUserIconPath(m_nAccountId);

				SendIcon(IconPath, api, m_nUserId);
			}
		}
		return true;
	}
	case XMSG_API_PROTOCOL_ID_GET_MESSAGE_SESSION: {
		api->SendProtoStatus(m_nUserId, true);
		uint32_t ProtoDataId = NULL;
		std::vector<char> m_FristUserNick = api->RecvData(ProtoDataId, m_nUserId);
		api->SendProtoStatus(m_nUserId, true);
		m_FristUserNick.push_back('\0');
		std::vector<char> m_SecondUserNick = api->RecvData(ProtoDataId, m_nUserId);
		api->SendProtoStatus(m_nUserId, true);
		m_SecondUserNick.push_back('\0');

		std::string m_sFristUserNick = m_FristUserNick.data();
		std::string m_sSecondUserNick = m_SecondUserNick.data();
		XMsg::Server::Session* m_Session = m_pDataBase->GetSession(m_sFristUserNick, m_sSecondUserNick);
		bool IsFirst = m_sFristUserNick == m_Session->FirstUser;

		api->SendNumLong(m_nUserId, m_Session->m_vMessages.size());

		if (IsFirst) {
			for (std::ptrdiff_t i = 0; i < m_Session->m_vMessages.size(); i++) {
				api->SendMsg(m_nUserId, m_Session->m_vMessages[i]);
				api->SendNum(m_nUserId, m_Session->m_vIsFirstUserMessage[i]);
			}
		}
		else {
			for (std::ptrdiff_t i = 0; i < m_Session->m_vMessages.size(); i++) {
				api->SendMsg(m_nUserId, m_Session->m_vMessages[i]);
				api->SendNum(m_nUserId, !m_Session->m_vIsFirstUserMessage[i]);
			}
		}

		return true;
	}
	case XMSG_API_PROTOCOL_ID_MSG: {
		api->SendProtoStatus(m_nUserId, true);

		std::ptrdiff_t m_nSessionUserId = api->RecvNumLong(m_nUserId);
		api->SendProtoStatus(m_nUserId, true);

		XPlatform::Api::u32 DataId;
		std::vector<char> m_vMessage = api->RecvData(DataId, m_nUserId);
		api->SendProtoStatus(m_nUserId, true);
		m_vMessage.push_back('\0');
		std::string m_sMessage = m_vMessage.data();

		std::string m_sFrom = m_pDataBase->GetAccountLogin(m_vUserData[m_nUserId].m_nAccountId);

		for (std::ptrdiff_t i = 0; i < m_vUserData.size(); i++) {
			if (m_vUserData[i].m_nAccountId == m_nSessionUserId) {
				api->SendProto(m_vUserData[i].m_nUserId, XMSG_API_PROTOCOL_ID_MSG);
				api->SendNum(m_vUserData[i].m_nUserId, m_vUserData[m_nUserId].m_nAccountId);
				api->SendData(m_vUserData[i].m_nUserId, m_sMessage.c_str(), m_sMessage.size());
			}
		}

		XMsg::Server::Session* m_pSession = m_pDataBase->GetSession(m_pDataBase->GetAccountLogin(m_vUserData[m_nUserId].m_nAccountId), m_pDataBase->GetAccountLogin(m_nSessionUserId));
		m_pSession->m_vMessages.push_back(m_sMessage);
		m_pSession->m_vIsFirstUserMessage.push_back(m_pSession->FirstUserId == m_vUserData[m_nUserId].m_nAccountId);

		return true;
	}
	case XMSG_API_PROTOCOL_ID_GET_FRIENDS_IDS:
		std::cout << "called: " << XMSG_API_PROTOCOL_ID_GET_FRIENDS_IDS << std::endl;
		api->SendProtoStatus(m_nUserId, m_vUserData[m_nUserId].IsAuth);
		if (m_vUserData[m_nUserId].IsAuth) {
			std::vector<std::ptrdiff_t>& m_vFriendsAccountsId = m_pDataBase->GetFriends(m_vUserData[m_nUserId].m_nAccountId);

			api->SendNumLong(m_nUserId, m_vFriendsAccountsId.size());

			for (std::ptrdiff_t m_nAccountId : m_vFriendsAccountsId) {
				api->SendNum(m_nUserId, m_nAccountId);
			}
		}
		return true;
	case XMSG_API_PROTOCOL_ID_GET_MY_ID:
		api->SendNumLong(m_nUserId, m_vUserData[m_nUserId].m_nAccountId);
		return true;
	case XMSG_API_PROTOCOL_ID_DISCONNECT:
		m_pDataBase->Disconnect(m_vUserData[m_nUserId].m_nAccountId);
		m_vUserData[m_nUserId].IsAuth = false;
		api->SendProtoStatus(m_nUserId, true);
		std::cout << "User dissconnected:" << m_nUserId << std::endl;
		m_vUserData[m_nUserId].IsAuth = false;
		return false;
	default: 
		m_pDataBase->Disconnect(m_vUserData[m_nUserId].m_nAccountId);
		m_vUserData[m_nUserId].IsAuth = false;
		api->SendProtoStatus(m_nUserId, true);
		std::cout << "User dissconnected:" << m_nUserId << std::endl;
		return false;
	}

	return false;
}

XMsg::Server::ProtocolHandler::~ProtocolHandler(){
	delete m_pDataBase;
}
