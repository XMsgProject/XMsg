#include <XPlatform.core/engine.h>
#include <XPlatform.core/loader.h>

#include<XFmt/XFmt.h>
#include<iostream>

#include "Application/Application.h"

void xplatform_MsgCallBack(const char* m_pMsg, XPlatform::Api::XPResult m_Res, XPlatform::Api::i32 ExtId) {
	const std::vector<XPlatform::core::XPlatformExtensionInfo>& ExtensionsInfoList =
		XPlatform::core::Engine::GetInstance()->GetExtensionsInfoList();

	if (ExtId == UINT32_MAX) {
		XPlatform::core::XPlatformExtensionInfo ExtInfo;
		ExtInfo.s_Name = "core";
		const XPlatform::core::XPlatformInformation* m_pXI = XPlatform::core::Engine::GetInstance()->GetEngineInfo();
		ExtInfo.s_Path = m_pXI->s_EngineDirectory;
		std::cout << "[" << ExtInfo.s_Name << "]" << "[info]" << m_pMsg << std::endl;
	}
	else {

		if (m_Res != XPlatform::Api::XPResult::XPLATFORM_RESULT_SUCCESS && m_Res != XPlatform::Api::XPResult::XPLATFORM_RESULT_NEUTRAL) {

#ifdef WIN32
			MessageBoxA(NULL, XFmt::Fmt("Error:\n\nExtId: %i\nPath: %s\nMessage: %s", ExtensionsInfoList[ExtId].ExtId, ExtensionsInfoList[ExtId].s_Path.c_str(), m_pMsg).c_str(), ExtensionsInfoList[ExtId].s_Name.c_str(), MB_ICONERROR);
#else
			std::cout << ExtInfo.s_Name << "\n" << XFmt::Fmt("Error:\nExtId: %i\nPath:\n%s\nMessage:\n%s", ExtensionsInfoList[ExtId].ExtId, ExtensionsInfoList[ExtId].s_Path, m_pMsg) << std::endl;
#endif

		}
		else {
			std::cout << "[" << ExtensionsInfoList[ExtId].s_Name << "]" << "[info]" << m_pMsg << std::endl;
		}
	}

}

int main(int argc, char* argv[]) {
	XPlatform::core::XPlatfromInit();
	XPlatform::core::Engine* m_pEngine = XPlatform::core::Engine::GetInstance();

	m_pEngine->SetMsgCallback(xplatform_MsgCallBack);

	XPlatform::Api::XPResult m_XRes = m_pEngine->LoadEngine("project.json");
	if (m_XRes != XPlatform::Api::XPResult::XPLATFORM_RESULT_SUCCESS)
		return -1;

	XMsg::Server::Application* m_pApp = new XMsg::Server::Application;

	if (!m_pApp->Init(argc, argv)) return -1;

	m_pApp->Run();

	delete m_pApp;

	XPlatform::core::XPlatfromShutdown();
	return 0;
}