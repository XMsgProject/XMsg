#ifndef _X_MSG_GUI_H
#define _X_MSG_GUI_H

#include "..//Window/Window.h"

#include<imgui.h>

namespace XMsg {
	namespace GUI {
		bool Initialize(const XMsg::Window& m_rWnd);
		void NewFrame() noexcept;

		void Show(XMsg::Window& m_rWnd) noexcept;

		void Shutdown() noexcept;

		ImFont* GetMassiveFont();

		class Form{
		public:
			virtual void SelectEvent() = 0;
			
			virtual void Render() = 0;
		};
	}
}

#endif