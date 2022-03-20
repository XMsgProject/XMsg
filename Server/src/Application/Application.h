#ifndef _XMSG_SERVER_APPLICATION_H
#define _XMSG_SERVER_APPLICATION_H

#include <unordered_map>
#include"..//Api/Api.h"
#include"..//ProtocolHandler/ProtocolHandler.h"

namespace XMsg {
	namespace Server {
		class Application {

			std::unordered_map<std::string, std::string> m_mssRuntimeVariables;

			XMsg::Server::Api* m_pApi;

			XMsg::Server::ProtocolHandler* m_pProtocolHanlder;

			std::uint16_t m_nPort = 443;
			std::string m_sPrivateKeyFile = "ssl/key.pem";
			std::string m_sPrivateKeyFilePass = "localhost";
			std::string m_sCertificateFile = "ssl/cert.pem";
			std::string m_sAddr = "127.0.0.1";
			bool m_bIsIPv6 = false;

			void ParseArgs(int argc, char* argv[]);

			
		public:
			Application();

			bool Init(int argc, char* argv[]);

			void Run();
		};
	}
}

#endif