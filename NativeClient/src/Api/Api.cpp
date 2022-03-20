#include "Api.h"

#include <XPlatform.core/Engine.h>
#include <XMsg.Api/XMsg.h>
#include <thread>

XMSG_API_INTERFACE* ApiInterface;
XMsg::Net::pfn_MessageCallback m_callback;

bool g_nIsConnectedToTheServer = false;
std::thread m_ListenThread;

bool XMsg::Net::Init(const char* IP, bool IsHostname, bool IsIPv6, uint16_t Port){
	XPlatform::core::XPlatformExtensionInfo* XMsgApiExtInfo = new XPlatform::core::XPlatformExtensionInfo;
	auto res = XPlatform::core::Engine::GetInstance()->GetExtensionInfo(XMSG_GET_API_EXT_NAME, XMsgApiExtInfo);
	if (res != XPlatform::Api::XPResult::XPLATFORM_RESULT_SUCCESS) {
		return false;
	}

	ApiInterface = reinterpret_cast<XMSG_API_INTERFACE * (*)()>(
		XPlatform::core::Engine::GetInstance()->GetExtensionModule(XMsgApiExtInfo->ExtId)->GetProc(XMSG_GET_API_INTERFACE_NAME)
		)();
	
	bool IsConnectedToTheServer = ApiInterface->ConnectToServer(IP, IsHostname, IsIPv6, Port) == XPlatform::Net::XPlatformNetResult::XPLATFORM_NET_RESULT_SUCCESS;

	g_nIsConnectedToTheServer = IsConnectedToTheServer;

	return IsConnectedToTheServer;
}

void XMsg::Net::SetMessageCallback(pfn_MessageCallback callback){
	m_callback = callback;
}

bool XMsg::Net::Auth(const std::string& Login, const std::string& Pass){
	bool m_bRes = ApiInterface->Login(Login, Pass);

	if (m_bRes) {
		XMsg::Net::Update = &XMsg::Net::Listen;
	}

	return m_bRes;
}

std::pair<bool, std::vector<std::string>> XMsg::Net::RequestFriendNames() {
	return ApiInterface->RequestFriendNames();
}

std::pair<bool, std::vector<XMSG_API_TEXTURE_DATA>> XMsg::Net::RequestFriendIcons(){
	return ApiInterface->RequestFriendIcons();
}

std::pair<bool, std::vector<_user_id_t>> XMsg::Net::RequestFriendIDs(){
	return ApiInterface->RequestFriendIds();
}

std::pair<bool, std::vector<std::pair<bool, std::string>>> XMsg::Net::LoadMessagesFromSession(const std::string YourNick, const std::string FriendNick){
	return ApiInterface->LoadMessagesFromSession(YourNick, FriendNick);
}

void XMsg::Net::x_SendMessage(const std::string& Message, const std::ptrdiff_t m_nSessionId){
	ApiInterface->SendProto(XMSG_API_PROTOCOL_ID_MSG);
	ApiInterface->SendNumLong(m_nSessionId);
	ApiInterface->SendData(XMSG_API_DATA_TYPE_ID_MESSAGE, Message.data(), Message.length());
}

void XMsg::Net::Disconnect(){
	if(g_nIsConnectedToTheServer)
	ApiInterface->Disconnect();
}

void XMsg::Net::Listen(){
	if (ApiInterface->Select()) {
		XPlatform::Api::u32 ProtoId = ApiInterface->RecvProto();

		switch (ProtoId){
		case XMSG_API_PROTOCOL_ID_DISCONNECT:
			ApiInterface->Disconnect();
			break;
		case XMSG_API_PROTOCOL_ID_MSG: {
			XPlatform::Api::u32 m_nSessionUserId = ApiInterface->RecvNum();
			std::string m_sMessage = ApiInterface->RecvMessage();
			m_callback(m_sMessage, m_nSessionUserId);
			break;
		}
		default:
			ApiInterface->Disconnect();
			break;
		}

	}
}

void m_UpdateLocal() {

}

void (*XMsg::Net::Update)() = m_UpdateLocal;
