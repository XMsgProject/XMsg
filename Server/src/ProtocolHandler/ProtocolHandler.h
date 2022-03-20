#ifndef _XMSG_SERVER_PROTOCOL_SERVER_H
#define _XMSG_SERVER_PROTOCOL_SERVER_H

#include "..//Api/Api.h"
#include "..//UserBase/UserBase.h"

namespace XMsg {
	namespace Server {
		class ProtocolHandler {

			XMsg::Server::UserBase* m_pDataBase;

			static XMsg::Server::ProtocolHandler* m_pInstance;
		public:
			ProtocolHandler();

			static XMsg::Server::ProtocolHandler* GetInstanse();

			XMsg::Server::UserBase* GetDataBase();

			void SendIcon(const std::string& IconPath, Api* api, std::ptrdiff_t m_nUserId);

			bool HandleProtocol(Api* api, XPlatform::Api::u32 m_nProtoId, std::ptrdiff_t m_nUserId);
			
			~ProtocolHandler();
		};
	}
}

#endif