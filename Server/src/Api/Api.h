#ifndef _XMSG_SERVER_API_H
#define _XMSG_SERVER_API_H

#include <XPlatform.core/api.h>
#include<XPlatform.core/engine.h>

#include <XPlatform.net/XP_ISocket.h>
#include<XPlatform.net/SSL/SSL.h>

#include <thread>

#define XMSG_API_PROTOCOL_ID_CONNECT 1
#define XMSG_API_PROTOCOL_ID_DISCONNECT 2
#define XMSG_API_PROTOCOL_ID_LOGIN 3
#define XMSG_API_PROTOCOL_ID_AUTH_STATUS 4
#define XMSG_API_PROTOCOL_ID_MSG 5
#define XMSG_API_PROTOCOL_ID_CHECK 6
#define XMSG_API_PROTOCOL_ID_GET_FRIENDS_NAMES 7
#define XMSG_API_PROTOCOL_ID_GET_FRIENDS_ICONS 8
#define XMSG_API_PROTOCOL_ID_GET_MESSAGE_SESSION 9
#define XMSG_API_PROTOCOL_ID_GET_FRIENDS_IDS 10
#define XMSG_API_PROTOCOL_ID_GET_MY_ID 11

namespace XMsg {
	namespace Server {
		class Api {
		public:

			typedef bool(*pfn_ProtocolHanlder)(Api* api, XPlatform::Api::u32 m_nProtoId, std::ptrdiff_t m_nUserId);

		private:

			std::vector<XPlatform::Net::XP_ISocket*> m_vClientsConnections;
			std::vector<std::thread> m_vThreadPool;

			std::vector<std::ptrdiff_t> m_vFreeSockets;

			XPlatform::core::XPlatformExtensionInfo m_xNetExtInfo;
			XPlatform::core::Engine* m_pEngine = XPlatform::core::Engine::GetInstance();

			XPlatform::Net::SSL::Ctx* m_pSSLCtx;

			XPlatform::Net::XP_ISocket* m_pxiListenSocket;
			std::ptrdiff_t m_nMaxUsersCount = 4;
			std::ptrdiff_t m_nCurrentUsersCount = 0;

			bool m_bIsApiThreadsNeedToWork = true;
			void ListenThread();

			void SetupClientHandler(std::ptrdiff_t m_nUserId);
			void ClientHandler(std::ptrdiff_t m_nUserId);
			pfn_ProtocolHanlder m_pProtocolHanlder;

			bool InitializeSSLContext(const std::string& CertificateFile, const std::string& PrivateKeyFile, const std::string& PrivateKeyPass);

		public:

			XMsg::Server::Api::pfn_ProtocolHanlder SetProtocolHandler(pfn_ProtocolHanlder Handler);

			void SendProto(const XPlatform::Api::u32 m_nClientId, const XPlatform::Api::u32 m_nProtoId);
			XPlatform::Api::u32 RecvProto(const XPlatform::Api::u32 m_nClientId);
			void SendProtoStatus(const XPlatform::Api::u32 m_nClientId, bool Status);

			std::ptrdiff_t RecvNumLong(const XPlatform::Api::u32 m_nClientId);

			void SendNum(const XPlatform::Api::u32 m_nClientId, const XPlatform::Api::u32 m_nNum);
			void SendNumLong(const XPlatform::Api::u32 m_nClientId, const std::ptrdiff_t m_nNum);

			void SendMsg(const XPlatform::Api::u32 m_nClientId, const std::string& m_sMsg);

			std::vector<char> RecvData(XPlatform::Api::u32 & DataProtoId, const XPlatform::Api::u32 m_nClientId);
			void SendData(XPlatform::Api::u32 UserId, const char* Data, int Len);

			std::string GetListenAddr();

			std::ptrdiff_t GetCurrentUserCount();
			std::ptrdiff_t GetFreeSocketsCount();

			void Run();
			void Shutdown();

			bool Init(const std::string& CertificateFile, const std::string& PrivateKeyFile, const std::string& PrivateKeyPass, const std::string& m_sIP, const bool m_pIPv6, const XPlatform::Api::u16 Port);
		};
	}
}

#endif