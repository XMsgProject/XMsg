#include "GUI.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include<backends/imgui_impl_glfw.h>
#include<backends/imgui_impl_opengl3.h>
#include <stdio.h>

#include <stdexcept>

#include"..//Font/Font.h"
#include <stdint.h>

#include<imgui_internal.h>
#include<stddef.h>

#define MASSIVE_FONT_SIZE 36
#define NORMAL_FONT_SIZE 23

static ImFont* MassiveFont = NULL;

bool XMsg::GUI::Initialize(const XMsg::Window& m_rWnd){
	int m_nGLVersion = gladLoadGL(glfwGetProcAddress);
	printf("[XMsg][GUI]:GL %i.%i", GLAD_VERSION_MAJOR(m_nGLVersion), GLAD_VERSION_MINOR(m_nGLVersion));

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->AddFontFromMemoryTTF(XMsg::GUI::Font, sizeof(XMsg::GUI::Font), NORMAL_FONT_SIZE, NULL, io.Fonts->GetGlyphRangesCyrillic());
	
	MassiveFont = 
	io.Fonts->AddFontFromMemoryTTF(XMsg::GUI::Font, sizeof(XMsg::GUI::Font), MASSIVE_FONT_SIZE, NULL, io.Fonts->GetGlyphRangesCyrillic());

	ImGuiContext& ctx = *ImGui::GetCurrentContext();
	ctx.FontAtlasOwnedByContext = false;


	ImGui::StyleColorsDark();

	//glfwSwapInterval(1);

	const char* glsl_version = "#version 130";
	bool res = ImGui_ImplGlfw_InitForOpenGL(m_rWnd.GetRawWindowHandle(), true);
	return res ? ImGui_ImplOpenGL3_Init(glsl_version) : false;
}

void XMsg::GUI::NewFrame() noexcept {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void XMsg::GUI::Show(XMsg::Window& m_rWnd) noexcept{
	ImGui::Render();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	m_rWnd.SwapBuffer();
}

void XMsg::GUI::Shutdown() noexcept{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

ImFont* XMsg::GUI::GetMassiveFont(){

	return MassiveFont;
}
