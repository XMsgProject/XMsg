#include "LoginForm.h"
#include <cmath>
#include <imgui_internal.h>

#include "../Api/Api.h"
#include "..//FormManager/FormManager.h"
#include <iostream>
#include <XPlatform.core\engine.h>
#include <XMsg.Api\XMsg.h>

#include "..//ChatForm/ChatForm.h"

#define V2 ImVec2
#define ARFM d->AddRectFilledMultiColor
#define ACF d->AddCircleFilled
#define CH s.SetCurrentChannel

void FX(ImDrawList* d, V2 a, V2 b, V2 sz, ImVec4, float t)
{
    ARFM(a, b, 0xFF1E1E1E, 0xFF1E281E, 0xFF1E1E5A, 0xFF321E78);
    ARFM(a, V2(b.x, a.y + sz.y * 0.4f), 0x1EFFDCFF, 0x14FFFFDC, 0x001E1E5A, 0x00321E78);
    auto s = d->_Splitter;
    s.Split(d, 2);
    for (int n = 0; n < 256; n++)
    {
        V2 c(a.x + n / 256.f * sz.x, b.y + 20 - cos(t - 0.1f * n / 2) * 10 + cos(t) * 5);
        float r(60 + sin(t + n / 2) * 40);
        CH(d, 0);
        ACF(c, r + 1, 0x80FFFFFF);
        CH(d, 1);
        ACF(c, r, IM_COL32(255, n / 2, n / 8, 255));
    }
    s.Merge(d);
    d->AddRect(a, b, IM_COL32(128, 128, 128, 100));
}

void XMsg::GUI::LoginForm::DrawBackGround(){
    ImGui::SetNextWindowPos(ImVec2(0, 0), 0, ImVec2(0, 0));

    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));

    ImGui::SetNextWindowBgAlpha(0);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::Begin("Background", NULL, ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar);

    ImVec2 size(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
    ImGui::InvisibleButton("canvas", size);
    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->PushClipRect(p0, p1);

    ImVec4 mouse_data;
    mouse_data.x = (io.MousePos.x - p0.x) / size.x;
    mouse_data.y = (io.MousePos.y - p0.y) / size.y;
    mouse_data.z = io.MouseDownDuration[0];
    mouse_data.w = io.MouseDownDuration[1];
    FX(draw_list, p0, p1, size, mouse_data, (float)ImGui::GetTime());
    draw_list->PopClipRect();

    //FX(BackGroundDrawList, ImVec2(640, 320), ImVec2(0, 0), ImVec2(640, 320), mouse_data, (float)ImGui::GetTime());

    ImGui::End();
    ImGui::PopStyleVar(2);
}

std::pair<uint16_t, std::string> ParseAddress(const char* Addr) {

    std::pair<uint16_t, std::string> Res;

    std::ptrdiff_t AddrLen = strlen(Addr);
    std::string sPort = "";

    std::ptrdiff_t i = 0;
    for (; i < AddrLen; i++) {
        if (Addr[i] == ':') break;
        Res.second.push_back(Addr[i]);
    }
    i++;

    for (; i < AddrLen; i++) {
        sPort.push_back(Addr[i]);
    }

    Res.first = atoi(sPort.c_str());

    return Res;
}

void XMsg::GUI::LoginForm::SelectEvent(){

}

XMsg::GUI::LoginForm::LoginForm(){
    memcpy(Address.data(), "127.0.0.1:443", 14);
    this->Login.fill(0);
    this->Pass.fill(0);
}

void XMsg::GUI::LoginForm::Render(){
    DrawBackGround();

    uint32_t ItemsWidth = io.DisplaySize.x * 0.5f;

    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2));
    ImGui::Begin("authorizing", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    
    ImGui::PushFont(XMsg::GUI::GetMassiveFont());
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth = ImGui::CalcTextSize("X Msg").x;
    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::TextColored(ImVec4(0.2, 0.2, 0.2, 1.0), "X");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.5, 0.5, 0.6, 1.0), "Msg");
    ImGui::PopFont();

    ImGui::Text(LANG_LOGIN);
    ImGui::SetNextItemWidth(ItemsWidth);
    ImGui::InputText("##Login", Login.data(), Login.size());
    ImGui::Text(LANG_PASS);
    ImGui::SetNextItemWidth(ItemsWidth);
    ImGui::InputText("##Password", Pass.data(), Pass.size());

    ImGui::Separator();
    ImGui::Text(LANG_ADDRESS);
    ImGui::InputText("##Address", Address.data(), Address.size());
    ImGui::Checkbox(LANG_IS_IP_V6, &IsIPv6);
    ImGui::SameLine();
    ImGui::Checkbox(LANG_IS_DOMAIN, &IsDomain);
    ImGui::Separator();
    
    if (ImGui::Button(LANG_LOG_IN, ImVec2(io.DisplaySize.x * 0.5f, 0.0f))) {
        if (!Login.empty() && !Pass.empty() && !Address.empty()) {
            auto ParseRes = ParseAddress(Address.data());
            if (!XMsg::Net::Init(ParseRes.second.c_str(), IsDomain, IsIPv6, ParseRes.first)) {
                std::size_t Id;
                const std::vector<XPlatform::core::XPlatformExtensionInfo>& InfoList = XPlatform::core::Engine::GetInstance()->GetExtensionsInfoList();
                for (const XPlatform::core::XPlatformExtensionInfo& info : InfoList) {
                    if (info.s_Name == XMSG_GET_API_EXT_NAME) {
                        Id = info.ExtId;
                        break;
                    }
                }
                XPlatform::core::Engine::GetInstance()->GetMsgCallback()("failed to connect to the server!", XPlatform::Api::XPResult::XPLATFORM_RESULT_FAILED, Id);
            }
            if (!XMsg::Net::Auth(Login.data(), Pass.data())) {
                std::cout << "failed to authificate!" << std::endl;
                XMsg::Net::Disconnect();
            }
            else {
                XMsg::GUI::FormManager::GetInstance()->SetCurrentForm(XMsg::GUI::FormId::_XMSG_FORM_ID_CHAT);
                reinterpret_cast<XMsg::GUI::ChatForm*>
                    (XMsg::GUI::FormManager::GetInstance()->GetCurrentForm())->SetNickName(Login.data());
            }

        }
    }

    ImGui::End();
}