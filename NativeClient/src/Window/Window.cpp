#include "Window.h"

#include<iostream>

#include<GLFW/glfw3.h>

XMsg::Window* XMsg::Window::m_pWnd = NULL;

#include <stdexcept>

XMsg::Window::Window(const char* m_pTitle, const uint32_t m_iWidth, const uint32_t m_iHeight){
	if (XMsg::Window::m_pWnd != NULL) {
		throw std::exception("[XMsg]: window instance is already created!");
	}

	m_pRawWindowHandle = glfwCreateWindow(m_iWidth, m_iHeight, m_pTitle, NULL, NULL);
	if (m_pRawWindowHandle == NULL) {
		throw std::exception("[XMsg]: failed to create window instance!");
	}

	glfwMakeContextCurrent(m_pRawWindowHandle);

	glfwSetWindowUserPointer(m_pRawWindowHandle, this);

	m_bIsFocused = true;
	glfwFocusWindow(m_pRawWindowHandle);

	XMsg::Window::m_pWnd = this;
}

bool XMsg::Window::IsFocused(){
	return glfwGetWindowAttrib(m_pRawWindowHandle, GLFW_FOCUSED);
}

bool XMsg::Window::IsFocusedOnShow(){
	return glfwGetWindowAttrib(m_pRawWindowHandle, GLFW_FOCUS_ON_SHOW);
}

bool XMsg::Window::IsIconified(){
	return glfwGetWindowAttrib(m_pRawWindowHandle, GLFW_ICONIFIED);
}

bool XMsg::Window::IsMaximized(){
	return 	glfwGetWindowAttrib(m_pRawWindowHandle, GLFW_MAXIMIZED);;
}

GLFWwindow* XMsg::Window::GetRawWindowHandle()const noexcept {
	return m_pRawWindowHandle;
}

bool XMsg::Window::IsWindowShouldClose() const noexcept {
	return glfwWindowShouldClose(m_pRawWindowHandle);
}

void XMsg::Window::SwapBuffer() noexcept {
	glfwSwapBuffers(m_pRawWindowHandle);
}

void XMsg::Window::PollEvents() noexcept{
	glfwPollEvents();
}

XMsg::Window::~Window(){
	glfwDestroyWindow(m_pRawWindowHandle);

	glfwTerminate();
}
