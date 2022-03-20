#include "Interface.h"

#include "..//XMsg.h"
#include <XPlatform.net/XP_ISocket.h>

#include <iostream>

XPlatform::Net::XP_ISocket* m_pxiSocket = NULL;
XPlatform::Net::SSL::Ctx* m_pCtx = NULL;

std::vector<char> m_RecvData();

XPlatform::Net::XPlatformNetResult m_ConnectToServer(const std::string& m_sIP, bool m_bIsHostname, bool m_bIPv6, XPlatform::Api::u16 m_nPort){
	if (m_pxiSocket != NULL) {
		XPlatform::core::Engine::GetInstance()->GetMsgCallback()("Connection is already openned!", XPlatform::Api::XPResult::XPLATFORM_RESULT_FAILED, XMsgApiExtInfo.ExtId);
		return XPlatform::Net::XPlatformNetResult::XPLATFORM_NET_RESULT_FAILED;
	}
	
	XPlatform::Net::XPlatformNetResult res = XPlatform::Net::XPlatformNetResult::XPLATFORM_NET_RESULT_SUCCESS;
	XPlatform::core::Engine* m_pEngine = XPlatform::core::Engine::GetInstance();

	XPlatform::Net::SSL::pfn_XPlatformCreateContextFunction XPlatformCreateContextFunction =
		reinterpret_cast<XPlatform::Net::SSL::pfn_XPlatformCreateContextFunction>(
			m_pEngine->GetExtensionModule(NetExtInfo.ExtId)->GetProc(XPlatform::Net::SSL::m_pXPlatformCreateContextFunctionName)
		);

	m_pCtx = XPlatformCreateContextFunction(true);

	XPlatformCreateSSLTCPSocketFunction =
		reinterpret_cast<XPlatform::Net::SSL::pfn_XPlatformCreateSSLTCPSocketFunction>(
			m_pEngine->GetExtensionModule(NetExtInfo.ExtId)->GetProc(m_bIPv6 ? XPlatform::Net::SSL::m_pXPlatformCreateSSLTCPSocketV6FunctionName : XPlatform::Net::SSL::m_pXPlatformCreateSSLTCPSocketFunctionName)
			);

	m_pxiSocket = XPlatformCreateSSLTCPSocketFunction(m_pCtx);

	std::string m_sfIP;

	if (m_bIsHostname)
		m_sfIP = XPlatformNetResolveHostnameToIP(m_sIP);
	else
		m_sfIP = m_sIP;

	if(m_sfIP.empty())return XPlatform::Net::XPlatformNetResult::XPLATFORM_NET_RESULT_FAILED;

	if (m_bIPv6)
		res = m_pxiSocket->Connect(XPlatform::Net::IPEndPoint6(m_sfIP, m_nPort));
	else
		res = m_pxiSocket->Connect(XPlatform::Net::IPEndPoint(m_sfIP, m_nPort));

	if (res != XPlatform::Net::XPlatformNetResult::XPLATFORM_NET_RESULT_SUCCESS) {
		m_pxiSocket->Close();
		m_pxiSocket->Release();
		m_pCtx->Release();
		m_pxiSocket = NULL;
		return res;
	}
	XPlatform::Api::u32 m_nEntryProto = XMsgApiInterface.RecvNum();
	if (m_nEntryProto == XMSG_API_PROTOCOL_ID_DISCONNECT) {
		std::string Message = XMsgApiInterface.RecvMessage();
		XPlatform::core::Engine::GetInstance()->GetMsgCallback()(Message.c_str(), XPlatform::Api::XPResult::XPLATFORM_RESULT_FAILED, XMsgApiExtInfo.ExtId);

		m_pxiSocket->Close();
		m_pxiSocket->Release();
		m_pCtx->Release();
		m_pxiSocket = NULL;
		return res;
	}

	return res;
}

void m_Disconnect(){
	XMsgApiInterface.SendProto(XMSG_API_PROTOCOL_ID_DISCONNECT);
	m_pxiSocket->Close();
	m_pxiSocket->Release();
	m_pCtx->Release();
	m_pxiSocket = NULL;
}

bool m_Login(const std::string& m_sLogin, const std::string& m_sPass){
	bool bRes = XMsgApiInterface.SendProto(XMSG_API_PROTOCOL_ID_LOGIN);
	if (!bRes) {
		XPlatform::core::Engine::GetInstance()->GetMsgCallback()("authentication on the server is disabled, please wait!", XPlatform::Api::XPResult::XPLATFORM_RESULT_FAILED, XMsgApiExtInfo.ExtId);

		return false;
	}

	if (XMsgApiInterface.SendProto(XMSG_API_PROTOCOL_ID_AUTH_STATUS)) {
		XPlatform::core::Engine::GetInstance()->GetMsgCallback()("you are already authentificated!", XPlatform::Api::XPResult::XPLATFORM_RESULT_FAILED, XMsgApiExtInfo.ExtId);
		XMsgApiInterface.Disconnect();

		return false;
	}

	XMsgApiInterface.SendData(XMSG_API_DATA_TYPE_ID_LOGIN, m_sLogin.c_str(), m_sLogin.length());
	XMsgApiInterface.SendData(XMSG_API_DATA_TYPE_ID_PASSWORD, m_sPass.c_str(), m_sPass.length());

	if (!XMsgApiInterface.SendProto(XMSG_API_PROTOCOL_ID_AUTH_STATUS)) {
		XPlatform::core::Engine::GetInstance()->GetMsgCallback()("authentication failed!", XPlatform::Api::XPResult::XPLATFORM_RESULT_FAILED, XMsgApiExtInfo.ExtId);

		return false;
	}

	return true;
}

std::pair<bool, std::vector<std::string>> m_RequestFriendsNames(){
	std::pair<bool, std::vector<std::string>> m_pbvFriendNamesList;
	m_pbvFriendNamesList.first = true;

	if (!XMsgApiInterface.SendProto(XMSG_API_PROTOCOL_ID_GET_FRIENDS_NAMES)) {
		XPlatform::core::Engine::GetInstance()->GetMsgCallback()("failed to get friend list: you are not authentificated!", XPlatform::Api::XPResult::XPLATFORM_RESULT_FAILED, XMsgApiExtInfo.ExtId);
		m_pbvFriendNamesList.first = false;
		m_pbvFriendNamesList.second = {};

		return m_pbvFriendNamesList;
	}

	std::ptrdiff_t i = XMsgApiInterface.RecvNumLong();
	for (; i > 0; i--) {
		std::string Name = XMsgApiInterface.RecvMessage();

		m_pbvFriendNamesList.second.push_back(Name);
	}

	return m_pbvFriendNamesList;
}

std::pair<bool, std::vector<_user_id_t>> m_RequestFriendsIds() {
	std::pair<bool, std::vector<_user_id_t>> m_pbvFriendNamesList;
	m_pbvFriendNamesList.first = true;

	if (!XMsgApiInterface.SendProto(XMSG_API_PROTOCOL_ID_GET_FRIENDS_IDS)) {
		XPlatform::core::Engine::GetInstance()->GetMsgCallback()("failed to get friend list: you are not authentificated!", XPlatform::Api::XPResult::XPLATFORM_RESULT_FAILED, XMsgApiExtInfo.ExtId);
		m_pbvFriendNamesList.first = false;
		m_pbvFriendNamesList.second = {};

		return m_pbvFriendNamesList;
	}

	std::ptrdiff_t i = XMsgApiInterface.RecvNumLong();
	for (; i > 0; i--) {
		_user_id_t Name = XMsgApiInterface.RecvNum();

		m_pbvFriendNamesList.second.push_back(Name);
	}

	return m_pbvFriendNamesList;
}

const std::ptrdiff_t m_RecvNumLong() {
	std::ptrdiff_t m_nBuffer;

	m_pxiSocket->Recv(reinterpret_cast<char*>(&m_nBuffer), sizeof(std::ptrdiff_t));
	return m_nBuffer;
}

std::pair<bool, std::vector<XMSG_API_TEXTURE_DATA>> m_RequestFriendsIcon() {
	std::pair<bool, std::vector<XMSG_API_TEXTURE_DATA>> m_pbvFriendsIconsList;
	m_pbvFriendsIconsList.first = true;

	if (!XMsgApiInterface.SendProto(XMSG_API_PROTOCOL_ID_GET_FRIENDS_ICONS)) {
		XPlatform::core::Engine::GetInstance()->GetMsgCallback()("failed to get friend list: you are not authentificated!", XPlatform::Api::XPResult::XPLATFORM_RESULT_FAILED, XMsgApiExtInfo.ExtId);
		m_pbvFriendsIconsList.first = false;
		m_pbvFriendsIconsList.second = {};

		return m_pbvFriendsIconsList;
	}

	std::ptrdiff_t i = XMsgApiInterface.RecvNumLong();
	
	for (; i > 0; i--) {
		XMSG_API_TEXTURE_DATA m_Data;

		m_Data.Width = XMsgApiInterface.RecvNum();
		m_Data.Height = XMsgApiInterface.RecvNum();
		m_Data.NrChannels = XMsgApiInterface.RecvNum();
		
		std::vector<char> Name = m_RecvData();
		std::cout << "recved bytes:" << Name.size() << std::endl;
		m_Data.Data = std::vector<unsigned char>(reinterpret_cast<unsigned char*>(Name.data()), reinterpret_cast<unsigned char*>(Name.data() + Name.size()));
		m_pbvFriendsIconsList.second.push_back(m_Data);
	}

	return m_pbvFriendsIconsList;
}

bool m_SendProto(const XPlatform::Api::u32 m_nProtoId){
	m_pxiSocket->Send(reinterpret_cast<const char*>(&m_nProtoId), sizeof(XPlatform::Api::u32));

	bool Byte = 0x0;
	m_pxiSocket->Recv(reinterpret_cast<char*>(&Byte), sizeof(bool));

	return Byte;
}

bool m_SendNumLong(const std::ptrdiff_t& m_nNumber){
	m_pxiSocket->Send(reinterpret_cast<const char*>(&m_nNumber), sizeof(std::ptrdiff_t));
	
	bool m_bProtoStatusByte = 0x0;
	m_pxiSocket->Recv(reinterpret_cast<char*>(&m_bProtoStatusByte), sizeof(bool));
		
	return m_bProtoStatusByte;
}

const XPlatform::Api::u32 m_RecvProto(){
	uint32_t Buff;
	m_pxiSocket->Recv(reinterpret_cast<char*>(&Buff), sizeof(uint32_t));
	return Buff;
}

bool m_SendData(const XPlatform::Api::u32 m_nDataInfo, const char* Data, int len){
	m_pxiSocket->Send(reinterpret_cast<const char*>(&m_nDataInfo), sizeof(XPlatform::Api::u32));
	m_pxiSocket->Send(reinterpret_cast<const char*>(&len), sizeof(int));
	m_pxiSocket->Send(Data, len);

	bool Byte = 0x0;
	m_pxiSocket->Recv(reinterpret_cast<char*>(&Byte), sizeof(bool));
	return Byte;
}

std::vector<char> m_RecvData() {
	std::vector<char> m_Res;
	int Len = 0;
	
	m_pxiSocket->Recv(reinterpret_cast<char*>(&Len), sizeof(int));
	m_Res.resize(Len + 1);
	m_pxiSocket->Recv(m_Res.data(), Len);

	return m_Res;
}

bool m_sSelect(){
	return m_pxiSocket->Select(10) == XPlatform::Net::XPlatformNetResult::XPLATFORM_NET_RESULT_SELECT_SUCCESS;
}

std::pair<bool, std::vector<std::pair<bool, std::string>>> m_LoadMessagesFromSession(const std::string YourNick, const std::string FriendNick){
	XMsgApiInterface.SendProto(XMSG_API_PROTOCOL_ID_GET_MESSAGE_SESSION);
	XMsgApiInterface.SendData(NULL, YourNick.c_str(), YourNick.length());
	XMsgApiInterface.SendData(NULL, FriendNick.c_str(), FriendNick.length());

	std::pair<bool, std::vector<std::pair<bool, std::string>>> m_Res;

	std::ptrdiff_t m_nCount = XMsgApiInterface.RecvNumLong();

	for (; m_nCount > 0; m_nCount--) {
		std::pair<bool, std::string> MsgSection;
		MsgSection.second = XMsgApiInterface.RecvMessage();
		MsgSection.first = XMsgApiInterface.RecvNum();

		m_Res.second.push_back(MsgSection);
	}

	return m_Res;
}

bool m_sMessage(const std::string& m_sMsg, XPlatform::Api::u32 To){
	XMsgApiInterface.SendProto(XMSG_API_PROTOCOL_ID_MSG);
	XMsgApiInterface.SendData(XMSG_API_DATA_TYPE_ID_NUM, reinterpret_cast<const char*>(&To), sizeof(XPlatform::Api::u32));
	XMsgApiInterface.SendData(XMSG_API_DATA_TYPE_ID_MESSAGE, m_sMsg.c_str(), m_sMsg.length());

	if (!XMsgApiInterface.SendProto(XMSG_API_PROTOCOL_ID_CHECK)) {
		std::string Msg = XMsgApiInterface.RecvMessage();
		XPlatform::core::Engine::GetInstance()->GetMsgCallback()(Msg.c_str(), XPlatform::Api::XPResult::XPLATFORM_RESULT_FAILED, XMsgApiExtInfo.ExtId);
		return false;
	}

	return true;
}

std::string m_RecvMessage() {
	const XPlatform::Api::u32 Length = XMsgApiInterface.RecvNum();
	char* Message = new char[Length + 2];
	std::memset(Message, 0, Length);
	m_pxiSocket->Recv(Message, Length);
	Message[Length] = 0x0;
	std::string Msg(Message);
	delete[] Message;
	return  Msg;
}
