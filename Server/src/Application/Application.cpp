#include "Application.h"
#include <iostream>

bool XMsgServerProtocolHanlder(XMsg::Server::Api* api, XPlatform::Api::u32 m_nProtoId, std::ptrdiff_t m_nUserId) {
	return XMsg::Server::ProtocolHandler::GetInstanse()->HandleProtocol(api, m_nProtoId, m_nUserId);
}

void XMsg::Server::Application::ParseArgs(int argc, char* argv[]){
	for (std::ptrdiff_t i = 1; i < argc; i++){
		std::string m_sVarName(argv[i]);
		i++;
		if (i > argc) break;
		std::string m_sVarVal(argv[i]);
		m_mssRuntimeVariables[m_sVarName] = m_sVarVal;
	}
}

XMsg::Server::Application::Application(){
	m_mssRuntimeVariables["-port"] = "443";
	m_mssRuntimeVariables["-IP"] = "127.0.0.1";
	m_mssRuntimeVariables["-IPv6"] = "f";
	m_mssRuntimeVariables["-PrivateKeyFilePass"] = "localhost";
	m_mssRuntimeVariables["-PrivateKeyFilePath"] = "ssl/key.pem";
	m_mssRuntimeVariables["-CertificateFilePath"] = "ssl/cert.pem";
	 
	m_pApi = new XMsg::Server::Api;
}

bool XMsg::Server::Application::Init(int argc, char* argv[]) {
	ParseArgs(argc, argv);
	
	m_nPort = atoi(m_mssRuntimeVariables["-port"].c_str());
	m_sAddr = m_mssRuntimeVariables["-IP"];
	m_bIsIPv6 = m_mssRuntimeVariables["-IPv6"][0] == 't';

	m_sCertificateFile = m_mssRuntimeVariables["-CertificateFilePath"];
	m_sPrivateKeyFile = m_mssRuntimeVariables["-PrivateKeyFilePath"];
	m_sPrivateKeyFilePass = m_mssRuntimeVariables["-PrivateKeyFilePass"];

	if (!m_pApi->Init(
		m_sCertificateFile,
		m_sPrivateKeyFile, 
		m_sPrivateKeyFilePass, 
		m_sAddr, 
		m_bIsIPv6, 
		m_nPort))
		return false;

	m_pApi->SetProtocolHandler(XMsgServerProtocolHanlder);

	m_pProtocolHanlder = new XMsg::Server::ProtocolHandler;
	
	m_pProtocolHanlder->GetDataBase()->RegisterRootUser("Root", "123");
	m_pProtocolHanlder->GetDataBase()->RegisterUser("TEST", "123");

	m_pProtocolHanlder->GetDataBase()->AddFriend(m_pProtocolHanlder->GetDataBase()->GetUserId("Root"), m_pProtocolHanlder->GetDataBase()->GetUserId("TEST"));

	return true;
}

void XMsg::Server::Application::Run(){
	m_pApi->Run();
	
	std::string m_sInput;
	while (true) {
		std::getline(std::cin, m_sInput);

		if (m_sInput == "CmdShowAddr") {
			std::cout << "[XMsg]: " << m_pApi->GetListenAddr() << std::endl;
		}else if (m_sInput == "CmdExit") {
			break;
		}
		else if (m_sInput == "CmdShowUserCounter") {
			std::cout << "[XMsg]: " << m_pApi->GetCurrentUserCount() << " Free socket count:" << m_pApi->GetFreeSocketsCount() << ";" << std::endl;
		}
	}

	m_pApi->Shutdown();

	delete m_pProtocolHanlder;
	delete m_pApi;
}
