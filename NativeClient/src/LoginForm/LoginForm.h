#ifndef _XMSG_LOGIN_FORM_H
#define _XMSG_LOGIN_FORM_H

#include"..//GUI/GUI.h"
#define IM_VEC2_CLASS_EXTRA ImVec2 operator*(int i) { return ImVec2(x * i, y*i); }\
							ImVec2 operator+(ImVec2 other) { return ImVec2(x + other.x, y += other.y); }
#include <imgui.h>
#include <array>

#ifdef _XMSG_LANG_EU
#include "langs/lang_eu.h"
#else
#include "langs/lang_ru.h"
#endif

namespace XMsg {
	namespace GUI {
		class LoginForm : public XMsg::GUI::Form{
			ImGuiIO& io = ImGui::GetIO();
			ImDrawList* BackGroundDrawList = ImGui::GetBackgroundDrawList();

			std::array<char, 256> Login;
			std::array<char, 256> Pass;

			std::array<char, 256> Address;

			bool IsIPv6 = false;
			bool IsDomain = false;

			void DrawBackGround();
		public:

			void SelectEvent();

			LoginForm();

			void Render();

		};
	}
}

#endif