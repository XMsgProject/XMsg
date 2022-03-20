#ifndef _XMSG_CHAT_FORM_H
#define _XMSG_CHAT_FORM_H

#include"..//GUI/GUI.h"
#include "..//Api/Api.h"
#include "..//Texture/Texture.h"

#include <imgui.h>
#include <array>

#include <vector>
#include <string>

namespace XMsg {
	namespace GUI {
		class ChatForm : public XMsg::GUI::Form {
			bool m_bIsFriendMenuOpen = false;
			bool m_bIsFriendMenuItemSelected = false;

			std::vector<std::string> m_vFriendList;
			std::vector<uint32_t> m_vFriendIDs;

			std::vector<XMsg::Gfx::Texture> m_vTextures;
			
			std::string UserNickName;

			void UpdateFriendList();

			struct Message {
				bool IsYour = false;
				std::string Text;
			};

			class FriendChatWindow {
				std::vector<Message> Msges;
				
				std::string FriendName = "";
				XMsg::Net::_user_id_t FriendId;
				bool IsOpen = false;

				void LoadMessages();

				std::array<char, 256> m_aMessageBuffer;

			public:
				ChatForm* m_ChatFromParent;

				void Init(const std::string& FriendName, const XMsg::Net::_user_id_t XMsgSystemId);

				void SendMessage(const std::string& Message);
				void RecvMessage(const std::string& Message);

				XMsg::Net::_user_id_t& GetFriendId();

				void Open();

				void Draw();

			};

			std::vector<FriendChatWindow*> FriendChatWindows;
			std::vector<std::string> m_OpenFriendsChats;

		public:

			ChatForm();

			void SetNickName(const std::string& Name);

			void OnFriendButtonClick(const std::string& Name);

			void MessageEvent(XPlatform::Api::u32 m_nSessionId, const std::string& m_sMessage);

			void SelectEvent();

			void Render();
		};
	}
}

#endif