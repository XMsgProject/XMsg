#ifndef _X_MSG_WINDOW_H
#define _X_MSG_WINDOW_H

struct GLFWwindow;

#include<stdint.h>

namespace XMsg {
	class Window {
		static Window* m_pWnd;
		bool m_bIsFocused = false;

		GLFWwindow* m_pRawWindowHandle;

	public:
		static XMsg::Window* GetInstace() { return XMsg::Window::m_pWnd; }

		Window(const char* m_pTitle, const uint32_t m_iWidth, const uint32_t m_iHeight);

		bool IsFocused();
		bool IsFocusedOnShow();
		bool IsIconified();
		bool IsMaximized();

		GLFWwindow* GetRawWindowHandle() const noexcept;
		bool IsWindowShouldClose() const noexcept;

		void SwapBuffer() noexcept;

		void PollEvents() noexcept;

		~Window();
	};
}

#endif