#include "Api.h"
#include <iostream>

void SendProtcol(XPlatform::Net::XP_ISocket* m_pxiSocket, uint32_t ProtoId) {
	m_pxiSocket->Send(reinterpret_cast<char*>(&ProtoId), sizeof(uint32_t));
}

void SendErrorProtocol(XPlatform::Net::XP_ISocket* m_pxiSocket, uint32_t ProtoId, std::string Message) {
	m_pxiSocket->Send(reinterpret_cast<char*>(&ProtoId), sizeof(uint32_t));
	uint32_t Length = Message.size();
	m_pxiSocket->Send(reinterpret_cast<char*>(&Length), sizeof(uint32_t));
	m_pxiSocket->Send(Message.c_str(), Length);
}

void XMsg::Server::Api::ListenThread(){


	while (m_bIsApiThreadsNeedToWork) {
		if (m_pxiListenSocket->Select(10) == XPlatform::Net::XPlatformNetResult::XPLATFORM_NET_RESULT_SELECT_SUCCESS) {
			XPlatform::Net::XP_ISocket* m_pxiTempClient = m_pxiListenSocket->Accept();
			if (m_pxiTempClient == NULL) continue;

			std::cout << "[XMsg]: new connection!" << std::endl;

			if (m_nMaxUsersCount <= m_nCurrentUsersCount) {
				SendErrorProtocol(m_pxiTempClient, XMSG_API_PROTOCOL_ID_DISCONNECT, "XMsg: server is full!");
				
				m_pxiTempClient->Close();
				m_pxiTempClient->Release();

				continue;
			}

			SendProtcol(m_pxiTempClient, XMSG_API_PROTOCOL_ID_CONNECT);

			if (!m_vFreeSockets.empty()) {
				std::ptrdiff_t m_nClientid;
				m_nClientid = m_vFreeSockets.back();
				m_vFreeSockets.pop_back();
				m_vClientsConnections[m_nClientid] = m_pxiTempClient;
				SetupClientHandler(m_nClientid);
				m_nCurrentUsersCount++;
				continue;
			}

			m_vClientsConnections.push_back(m_pxiTempClient);

			SetupClientHandler(m_nCurrentUsersCount);

			m_nCurrentUsersCount++;
		}
	}
}

void XMsg::Server::Api::SetupClientHandler(std::ptrdiff_t m_nUserId){
	m_vThreadPool.push_back(std::thread(&XMsg::Server::Api::ClientHandler, this, m_nUserId));
}

void XMsg::Server::Api::ClientHandler(std::ptrdiff_t m_nUserId){
	XPlatform::Net::XP_ISocket* m_pxiClient = m_vClientsConnections[m_nUserId];
	while (m_bIsApiThreadsNeedToWork){
		if (m_pxiClient->Select(5) == XPlatform::Net::XPlatformNetResult::XPLATFORM_NET_RESULT_SELECT_SUCCESS) {
			auto ProtoId = RecvProto(m_nUserId);
			if (!m_pProtocolHanlder(this, ProtoId, m_nUserId)) {
				m_pxiClient->Close();
				m_vFreeSockets.push_back(m_nUserId);
				m_nCurrentUsersCount--;
				return;
			}
		}
	}
}

bool XMsg::Server::Api::InitializeSSLContext(const std::string& CertificateFile, const std::string& PrivateKeyFile, const std::string& PrivateKeyPass) {
	XPlatform::Net::SSL::pfn_XPlatformCreateContextFunction XPlatformCreateContext =
		reinterpret_cast<XPlatform::Net::SSL::pfn_XPlatformCreateContextFunction>(
			m_pEngine->GetExtensionModule(m_xNetExtInfo.ExtId)->GetProc(XPlatform::Net::SSL::m_pXPlatformCreateContextFunctionName)
			);
	
	m_pSSLCtx = XPlatformCreateContext(false);
	if (!m_pSSLCtx->SetCertificateFile(CertificateFile, XPlatform::Net::SSL::SSLFileType::PEM))
		return false;

	m_pSSLCtx->SetDefualtPasswordCallbackUserData((void*)PrivateKeyPass.data());
	if (!m_pSSLCtx->SetPrivateKeyFile(PrivateKeyFile, XPlatform::Net::SSL::SSLFileType::PEM))
		return false;

	return true;
}

XMsg::Server::Api::pfn_ProtocolHanlder XMsg::Server::Api::SetProtocolHandler(pfn_ProtocolHanlder Handler){
	pfn_ProtocolHanlder m_TmpOld = m_pProtocolHanlder;
	m_pProtocolHanlder = Handler;

	return m_TmpOld;
}

void XMsg::Server::Api::SendProto(const XPlatform::Api::u32 m_nClientId, const XPlatform::Api::u32 m_nProtoId){
	m_vClientsConnections[m_nClientId]->Send(reinterpret_cast<const char*>(&m_nProtoId), sizeof(uint32_t));
}

XPlatform::Api::u32 XMsg::Server::Api::RecvProto(const XPlatform::Api::u32 m_nClientId){
	XPlatform::Api::u32 Buff = 0x0;
	m_vClientsConnections[m_nClientId]->Recv(reinterpret_cast<char*>(&Buff), sizeof(uint32_t));
	return Buff;
}

void XMsg::Server::Api::SendProtoStatus(const XPlatform::Api::u32 m_nClientId, bool Status) {
	m_vClientsConnections[m_nClientId]->Send(reinterpret_cast<const char*>(&Status), sizeof(bool));
}

std::ptrdiff_t XMsg::Server::Api::RecvNumLong(const XPlatform::Api::u32 m_nClientId){
	std::ptrdiff_t m_nBuffer = 0x0;
	m_vClientsConnections[m_nClientId]->Recv(reinterpret_cast<char*>(&m_nBuffer), sizeof(std::ptrdiff_t));

	return m_nBuffer;
}

void XMsg::Server::Api::SendNum(const XPlatform::Api::u32 m_nClientId, const XPlatform::Api::u32 m_nNum){
	m_vClientsConnections[m_nClientId]->Send(reinterpret_cast<const char*>(&m_nNum), sizeof(XPlatform::Api::u32));
}

void XMsg::Server::Api::SendNumLong(const XPlatform::Api::u32 m_nClientId, const std::ptrdiff_t m_nNum){
	m_vClientsConnections[m_nClientId]->Send(reinterpret_cast<const char*>(&m_nNum), sizeof(std::ptrdiff_t));
}

void XMsg::Server::Api::SendMsg(const XPlatform::Api::u32 m_nClientId, const std::string& m_sMsg){
	XPlatform::Api::u32 Length = m_sMsg.length();

	m_vClientsConnections[m_nClientId]->Send(reinterpret_cast<char*>(&Length), sizeof(XPlatform::Api::u32));
	m_vClientsConnections[m_nClientId]->Send(m_sMsg.data(), Length);
}

std::vector<char> XMsg::Server::Api::RecvData(XPlatform::Api::u32& DataProtoId, const XPlatform::Api::u32 m_nClientId){

	m_vClientsConnections[m_nClientId]->Recv(reinterpret_cast<char*>(&DataProtoId), sizeof(XPlatform::Api::u32));
	int DataLen = 0;
	m_vClientsConnections[m_nClientId]->Recv(reinterpret_cast<char*>(&DataLen), sizeof(int));

	std::vector<char> m_vData;
	m_vData.resize(DataLen);
	m_vClientsConnections[m_nClientId]->Recv(m_vData.data(), DataLen);

	return m_vData;
}

void XMsg::Server::Api::SendData(XPlatform::Api::u32 m_nClientId, const char* Data, int Len){
	m_vClientsConnections[m_nClientId]->Send(reinterpret_cast<char*>(&Len), sizeof(int));
	m_vClientsConnections[m_nClientId]->Send(Data, Len);
}

std::string XMsg::Server::Api::GetListenAddr(){
	return std::string("ssl://") + m_pxiListenSocket->GetEndPoint().GetIP() + std::to_string(m_pxiListenSocket->GetEndPoint().GetPort());
}

std::ptrdiff_t XMsg::Server::Api::GetCurrentUserCount(){
	return m_nCurrentUsersCount;
}

std::ptrdiff_t XMsg::Server::Api::GetFreeSocketsCount(){
	return m_vFreeSockets.size();
}

void XMsg::Server::Api::Run(){
	m_vThreadPool.push_back(std::thread(&XMsg::Server::Api::ListenThread, this));
}

void XMsg::Server::Api::Shutdown(){
	this->m_bIsApiThreadsNeedToWork = false;

	for (std::thread& thread : m_vThreadPool) {
		while (!thread.joinable()) {

		}
		thread.join();
	}
	
	for (XPlatform::Net::XP_ISocket* m_pSock : m_vClientsConnections) {
		SendErrorProtocol(m_pSock, XMSG_API_PROTOCOL_ID_DISCONNECT, "XMsg: server is stopped!");
		m_pSock->Close();
		m_pSock->Release();
		m_pSock = NULL;
	}

	m_pxiListenSocket->Release();
	m_pSSLCtx->Release();
}

bool XMsg::Server::Api::Init(const std::string& CertificateFile, const std::string& PrivateKeyFile, const std::string& PrivateKeyPass, const std::string& m_sIP, const bool m_IPv6, const XPlatform::Api::u16 Port){
	const std::vector<XPlatform::core::XPlatformExtensionInfo>& ExtList = m_pEngine->GetExtensionsInfoList();

	for (const XPlatform::core::XPlatformExtensionInfo& ExtInfo : ExtList) {
		if (ExtInfo.s_Name == XPLATFORM_NET_EXT_NAME) {
			m_xNetExtInfo = ExtInfo;
		}
	}

	XPlatform::Net::SSL::pfn_XPlatformCreateSSLTCPSocketFunction XPlatformCreateSSLTCPSocket =
		reinterpret_cast<XPlatform::Net::SSL::pfn_XPlatformCreateSSLTCPSocketFunction>(
			m_pEngine->GetExtensionModule(m_xNetExtInfo.ExtId)->GetProc(m_IPv6 ? XPlatform::Net::SSL::m_pXPlatformCreateSSLTCPSocketV6FunctionName : XPlatform::Net::SSL::m_pXPlatformCreateSSLTCPSocketFunctionName)
		);

	if (!InitializeSSLContext(CertificateFile, PrivateKeyFile, PrivateKeyPass)) {
		return false;
	}

	m_pxiListenSocket = XPlatformCreateSSLTCPSocket(m_pSSLCtx);

	XPlatform::Net::XPlatformNetResult m_nNetRes;
	
	if (m_IPv6)
		m_nNetRes = m_pxiListenSocket->Listen(XPlatform::Net::IPEndPoint6(m_sIP, Port));
	else
		m_nNetRes = m_pxiListenSocket->Listen(XPlatform::Net::IPEndPoint(m_sIP, Port));

	if (m_nNetRes != XPlatform::Net::XPlatformNetResult::XPLATFORM_NET_RESULT_SUCCESS)
		return false;

	return true;
}
