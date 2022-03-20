#include "Window/Window.h"
#include "GUI/GUI.h"

#include<imgui.h>

#include<stdio.h>
#include <GLFW/glfw3.h>

#include"LoginForm/LoginForm.h"
#include "Api/Api.h"

#include <XPlatform.core/api.h>
#include <XPlatform.core/engine.h>
#include <XPlatform.core/loader.h>

#include <iostream>
#include <XFmt/XFmt.h>

#include <XMsg.Api/XMsg.h>

#include "FormManager\FormManager.h"

#include <ctime>
#include <chrono>
#include <iostream>


static void glfw_error_callback(int error, const char* description){
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
void xplatform_MsgCallBack(const char* m_pMsg, XPlatform::Api::XPResult m_Res, XPlatform::Api::i32 ExtId) {
	XPlatform::core::XPlatformExtensionInfo ExtInfo;
	const std::vector<XPlatform::core::XPlatformExtensionInfo>& ExtensionsInfoList =
		XPlatform::core::Engine::GetInstance()->GetExtensionsInfoList();

	if (ExtId == UINT32_MAX) {
		ExtInfo.s_Name = "core";
		const XPlatform::core::XPlatformInformation* m_pXI = XPlatform::core::Engine::GetInstance()->GetEngineInfo();

		ExtInfo.s_Path = m_pXI->s_EngineDirectory;
	}
	else {
		ExtInfo = ExtensionsInfoList[ExtId];

		if (m_Res != XPlatform::Api::XPResult::XPLATFORM_RESULT_SUCCESS && m_Res != XPlatform::Api::XPResult::XPLATFORM_RESULT_NEUTRAL) {

#ifdef WIN32
			MessageBoxA(NULL, XFmt::Fmt("Error:\n\nExtId: %i\nPath: %s\nMessage: %s", ExtInfo.ExtId, ExtInfo.s_Path.c_str(), m_pMsg).c_str(), ExtInfo.s_Name.c_str(), MB_ICONERROR);
#else
			std::cout << ExtInfo.s_Name << "\n" << XFmt::Fmt("Error:\nExtId: %i\nPath:\n%s\nMessage:\n%s", ExtInfo.ExtId, ExtInfo.s_Path, m_pMsg) << std::endl;
#endif

		}
		else {
			std::cout << "[" << ExtInfo.s_Name << "]" << "[info]" << m_pMsg << std::endl;
		}
	}
}

int main(int argc, char* argv[]) {
	glfwSetErrorCallback(glfw_error_callback);
	if (glfwInit() != GLFW_TRUE) {
		return -1;
	}

	XPlatform::core::XPlatfromInit();

	XPlatform::core::Engine::GetInstance()->SetMsgCallback(xplatform_MsgCallBack);

	XPlatform::core::Engine* m_pEngine = XPlatform::core::Engine::GetInstance();

	XPlatform::Api::XPResult res = m_pEngine->LoadEngine("project.json");
	if (res != XPlatform::Api::XPResult::XPLATFORM_RESULT_SUCCESS) {
		return -1;
	}
	
	XMsg::Window m_wWindowInstance("XMsg", 840, 640);
	glfwSwapInterval(1);

	if (!XMsg::GUI::Initialize(m_wWindowInstance))
		return -1;
	
	XMsg::GUI::FormManager* m_pFormManager = new XMsg::GUI::FormManager;
	m_pFormManager->SetCurrentForm(XMsg::GUI::FormId::_XMSG_FORM_ID_LOGIN);

	while (!m_wWindowInstance.IsWindowShouldClose()) {
		//auto start = std::chrono::steady_clock::now();

		XMsg::Net::Update();

		m_wWindowInstance.PollEvents();
		if (glfwGetKey(m_wWindowInstance.GetRawWindowHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(m_wWindowInstance.GetRawWindowHandle(), true);
		//if (glfwGetKey(m_wWindowInstance.GetRawWindowHandle(), GLFW_KEY_T) == GLFW_PRESS) glfwSetWindowAttrib(m_wWindowInstance.GetRawWindowHandle(), GLFW_DECORATED, GLFW_FALSE);

		XMsg::GUI::NewFrame();
		if(!m_wWindowInstance.IsIconified())
		m_pFormManager->GetCurrentForm()->Render();

		XMsg::GUI::Show(m_wWindowInstance);
		//auto end = std::chrono::steady_clock::now();
		//auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		//std::cout << "It took me " << elapsed.count() << " microseconds." << std::endl;
	}
	
	XMsg::Net::Disconnect();

	delete m_pFormManager;

	XMsg::GUI::Shutdown();

	XPlatform::core::XPlatfromShutdown();
	return 0;
}
