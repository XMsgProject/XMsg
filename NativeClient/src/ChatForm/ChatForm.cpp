#include "ChatForm.h"

#include "..//FormManager/FormManager.h"

#ifdef _XMSG_LANG_EU
#include "langs/lang_eu.h"
#else
#include "langs/lang_ru.h"
#endif
#include <iostream>
#include <fstream>

#include "..//stb_image/stb_image.h"
#include "..//stb_image_write/stb_image_write.h"

void m_MsgCallback(const std::string& Msg, XPlatform::Api::u32 m_nSessionUserId) {
	XMsg::GUI::ChatForm* m_pForm = reinterpret_cast<XMsg::GUI::ChatForm*>(XMsg::GUI::FormManager::GetInstance()->GetForm(XMsg::GUI::FormId::_XMSG_FORM_ID_CHAT));
	m_pForm->MessageEvent(m_nSessionUserId, Msg);
}

void XMsg::GUI::ChatForm::UpdateFriendList(){
	auto RequestRes = XMsg::Net::RequestFriendNames();
	if (!RequestRes.first) {
		m_vFriendList.push_back("ERROR:505");
		return;
	}

	m_vFriendList = RequestRes.second;

	auto FriendIdsRes = XMsg::Net::RequestFriendIDs();
	
	m_vFriendIDs = FriendIdsRes.second;

	XMsg::Net::SetMessageCallback(m_MsgCallback);
}

XMsg::GUI::ChatForm::ChatForm() {

}

void XMsg::GUI::ChatForm::SetNickName(const std::string& Name){
	UserNickName = Name;
}

void XMsg::GUI::ChatForm::OnFriendButtonClick(const std::string& Name){
	if (std::find(m_OpenFriendsChats.begin(), m_OpenFriendsChats.end(), Name) != m_OpenFriendsChats.end()) {
		
		std::ptrdiff_t i = 0;
		for (; i < m_OpenFriendsChats.size(); i ++ ) {
			if (m_OpenFriendsChats[i] == Name) {
				FriendChatWindows[i]->Open();
				break;
			}
		}

	}
	else {
		m_OpenFriendsChats.push_back(Name);
		XMsg::GUI::ChatForm::FriendChatWindow* m_pWindow = new XMsg::GUI::ChatForm::FriendChatWindow;
		m_pWindow->m_ChatFromParent = this;
		m_pWindow->Open();
		
		XMsg::Net::_user_id_t FriendId;

		for (std::ptrdiff_t i = 0; i < m_vFriendList.size(); i++) {
			if (m_vFriendList[i] == Name)
				FriendId = m_vFriendIDs[i];
		}

		m_pWindow->Init(Name, FriendId);
		FriendChatWindows.push_back(m_pWindow);
	}
}

void XMsg::GUI::ChatForm::MessageEvent(XPlatform::Api::u32 m_nSessionId, const std::string& m_sMessage){
	for (XMsg::GUI::ChatForm::FriendChatWindow* m_pFriendChatWindow : FriendChatWindows){
		if (m_pFriendChatWindow->GetFriendId() == m_nSessionId) {
			m_pFriendChatWindow->RecvMessage(m_sMessage);
		}
	}
}

void XMsg::GUI::ChatForm::SelectEvent() {
	UpdateFriendList();
}

void XMsg::GUI::ChatForm::Render() {

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu(LANG_WINDOWS, true)) {
			ImGui::MenuItem(LANG_FRIENDS, LANG_OPEN_FRIEND_LIST, &m_bIsFriendMenuOpen, true);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (m_bIsFriendMenuOpen) {
		ImGui::Begin(LANG_FRIENDS, &m_bIsFriendMenuOpen);

		for (std::int32_t i = 0; i < m_vFriendList.size(); i++) {

			/*ImGui::Image(
				reinterpret_cast<ImTextureID>(m_Text.GetId()),
				ImVec2(m_Text.GetWidth(), m_Text.GetHeight()));*/

			if (ImGui::Button(m_vFriendList[i].c_str())) {
				this->OnFriendButtonClick(m_vFriendList[i]);
			}
		}

		/*
		for (std::string& FriendName : m_vFriendList) {
			if (ImGui::Button(FriendName.c_str())) {
				this->OnFriendButtonClick(FriendName);
			}
		}*/

		ImGui::End();
	}

	for (FriendChatWindow* m_Wnd : this->FriendChatWindows) {
		m_Wnd->Draw();
	}
}

void XMsg::GUI::ChatForm::FriendChatWindow::LoadMessages(){
	auto Res = XMsg::Net::LoadMessagesFromSession(m_ChatFromParent->UserNickName, FriendName);

	for (std::pair<bool, std::string> m_Section : Res.second) {
		Msges.push_back(Message{ m_Section.first, m_Section.second });
	}
}

void XMsg::GUI::ChatForm::FriendChatWindow::Init(const std::string& FriendName, const XMsg::Net::_user_id_t m_nFriendId){
	this->FriendName = FriendName;
	this->FriendId = m_nFriendId;
	m_aMessageBuffer.fill(0);


	LoadMessages();
}

void XMsg::GUI::ChatForm::FriendChatWindow::SendMessage(const std::string& Message){

	XMsg::GUI::ChatForm::Message m_sMessage;
	m_sMessage.IsYour = true;
	m_sMessage.Text = Message;
	Msges.push_back(m_sMessage);

	XMsg::Net::x_SendMessage(Message, FriendId);
}

void XMsg::GUI::ChatForm::FriendChatWindow::RecvMessage(const std::string& Message){
	XMsg::GUI::ChatForm::Message m_sMessage;
	m_sMessage.IsYour = false;
	m_sMessage.Text = Message;

	Msges.push_back(m_sMessage);
}

XMsg::Net::_user_id_t& XMsg::GUI::ChatForm::FriendChatWindow::GetFriendId(){
	return FriendId;
}

void XMsg::GUI::ChatForm::FriendChatWindow::Open(){
	IsOpen = true;
}

size_t GetSizeDiff(size_t size, int diff) {
	size_t m_nDebugVar = size < diff ? 0 : size - diff;

	return m_nDebugVar;
}

void XMsg::GUI::ChatForm::FriendChatWindow::Draw(){
	if (IsOpen) {
		ImGui::Begin(FriendName.c_str(), &IsOpen);

		size_t m_nMessagesSize = Msges.size();

		for (size_t i = GetSizeDiff(m_nMessagesSize, !m_bMessagesRestriction ? m_nNumShowMessage : m_nMessagesSize); i < m_nMessagesSize; i++) {
			Message& m_Msg = Msges[i];
			
			if (m_Msg.IsYour) {
				ImGui::Text(m_ChatFromParent->UserNickName.c_str());
				ImGui::Text(m_Msg.Text.c_str());
				ImGui::Separator();
			}
			else {
				ImGui::Text(FriendName.c_str());
				ImGui::Text(m_Msg.Text.c_str());
				ImGui::Separator();
			}
		}

		if (ImGui::InputText("##MessageInput", m_aMessageBuffer.data(), m_aMessageBuffer.size(), ImGuiInputTextFlags_::ImGuiInputTextFlags_CtrlEnterForNewLine | ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue)) {
			if (m_aMessageBuffer.empty());
			this->SendMessage(m_aMessageBuffer.data());
			m_aMessageBuffer.fill(0);
		}

		ImGui::SameLine();
		
		if (ImGui::Button("Send")) {
			if (m_aMessageBuffer.empty());
			this->SendMessage(m_aMessageBuffer.data());
			m_aMessageBuffer.fill(0);
		}

		ImGui::Separator();

		ImGui::Checkbox("disable message restriction", &m_bMessagesRestriction);

		if (!m_bMessagesRestriction) {
			ImGui::DragInt("number of showing messages", &m_nNumShowMessage, 0.5f, 4, 10000);
		}

		ImGui::End();
	}
}
