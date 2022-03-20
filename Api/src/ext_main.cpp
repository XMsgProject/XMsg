#include "XMsg.h"

XPlatform::core::XPlatformExtensionInfo NetExtInfo;
XPlatform::core::XPlatformExtensionInfo XMsgApiExtInfo;
XMSG_API_INTERFACE XMsgApiInterface;

XPlatform::Net::pfn_XPlatformNetResolveHostnameToIP XPlatformNetResolveHostnameToIP;
XPlatform::Net::SSL::pfn_XPlatformCreateContextFunction XPlatformCreateContextFunction;
XPlatform::Net::SSL::pfn_XPlatformCreateSSLTCPSocketFunction XPlatformCreateSSLTCPSocketFunction;
XPlatform::Net::SSL::pfn_XPlatformCreateSSLTCPSocketFunction XPlatformCreateSSLTCPSocketv6Function;

#include "Interface/Interface.h"

#include <XFmt/XFmt.h>

template<class t_type>
t_type SafeGetProc(const uint32_t ExtId, const std::string& m_rName) {
	t_type val = reinterpret_cast<t_type>(
		XPlatform::core::Engine::GetInstance()->GetExtensionModule(ExtId)->GetProc(m_rName)
	);

	if (val == NULL) {
		XPlatform::core::Engine::GetInstance()->GetMsgCallback()(
			"", 
			XPlatform::Api::XPResult::XPLATFORM_RESULT_FAILED, 
			ExtId);
	}

	return val;
}

extern "C" XPLATFORM_API_EXPORT XMSG_API_INTERFACE* XMsgApiGetInterface() {
	return &XMsgApiInterface;
}

XMSG_EXT_API XPlatform::Api::XPResult xplatform_extension_init() {

	XPlatform::core::Engine* m_pEngine = XPlatform::core::Engine::GetInstance();

	XPlatform::Api::XPResult res = m_pEngine->GetExtensionInfo(XPLATFORM_NET_EXT_NAME, &NetExtInfo);
	if (res != XPlatform::Api::XPResult::XPLATFORM_RESULT_SUCCESS) {

		return XPlatform::Api::XPResult::XPLATFORM_RESULT_FAILED;
	}

	XPlatformNetResolveHostnameToIP = reinterpret_cast<XPlatform::Net::pfn_XPlatformNetResolveHostnameToIP>(
		m_pEngine->GetExtensionModule(NetExtInfo.ExtId)->GetProc(XPlatform::Net::XPlatformNetResolveHostnameToIP)
	);

	XPlatformCreateContextFunction = reinterpret_cast<XPlatform::Net::SSL::pfn_XPlatformCreateContextFunction>(
		m_pEngine->GetExtensionModule(NetExtInfo.ExtId)->GetProc(XPlatform::Net::SSL::m_pXPlatformCreateContextFunctionName)
	);

	XPlatformCreateSSLTCPSocketFunction = reinterpret_cast<XPlatform::Net::SSL::pfn_XPlatformCreateSSLTCPSocketFunction>(
		m_pEngine->GetExtensionModule(NetExtInfo.ExtId)->GetProc(XPlatform::Net::SSL::m_pXPlatformCreateSSLTCPSocketFunctionName)
	);

	XPlatformCreateSSLTCPSocketv6Function = reinterpret_cast<XPlatform::Net::SSL::pfn_XPlatformCreateSSLTCPSocketFunction>(
		m_pEngine->GetExtensionModule(NetExtInfo.ExtId)->GetProc(XPlatform::Net::SSL::m_pXPlatformCreateSSLTCPSocketV6FunctionName)
	);

	// api 0.1 functions
	XMsgApiInterface.ConnectToServer = m_ConnectToServer;
	XMsgApiInterface.Disconnect = m_Disconnect;
	XMsgApiInterface.Login = m_Login;
	XMsgApiInterface.Message = m_sMessage;
	XMsgApiInterface.RecvMessage = m_RecvMessage;
	XMsgApiInterface.RecvNum = m_RecvProto;
	XMsgApiInterface.RecvProto = m_RecvProto;
	XMsgApiInterface.SendProto = m_SendProto;
	XMsgApiInterface.Select = m_sSelect;
	XMsgApiInterface.SendData = m_SendData;

	// api 0.2 functions
	XMsgApiInterface.RecvNumLong = m_RecvNumLong;
	XMsgApiInterface.SendNumLong = m_SendNumLong;

	// api 0.1 functions
	XMsgApiInterface.RequestFriendNames = m_RequestFriendsNames;
	XMsgApiInterface.RequestFriendIcons = m_RequestFriendsIcon;
	XMsgApiInterface.LoadMessagesFromSession = m_LoadMessagesFromSession;
	
	//api 0.2 functions
	XMsgApiInterface.RequestFriendIds = m_RequestFriendsIds;

	return XPlatform::Api::XPResult::XPLATFORM_RESULT_SUCCESS;
}

XMSG_EXT_API void xplatform_extension_shutdown() {

}


