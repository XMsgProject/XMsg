#ifndef _XMSG_INTERFACE_H
#define _XMSG_INTERFACE_H

#include <XPlatform.core/engine.h>
#include <XPlatform.core/api.h>

#include <XPlatform.net/Net.h>
#include <XPlatform.net/SSL/SSL.h>
#include <vector>
#include <utility>

#include "..//XMsg.h"

XPlatform::Net::XPlatformNetResult m_ConnectToServer
(const std::string& m_sIP, bool m_bIsHostname, bool m_bIPv6, XPlatform::Api::u16 m_nPort);

void m_Disconnect();

bool m_Login(const std::string& m_sLogin, const std::string& m_sPass);

std::pair<bool,std::vector<std::string>> m_RequestFriendsNames();
std::pair<bool,std::vector<_user_id_t>> m_RequestFriendsIds();
std::pair<bool, std::vector<XMSG_API_TEXTURE_DATA>> m_RequestFriendsIcon();

bool m_SendProto(const XPlatform::Api::u32 m_nProtoId);
bool m_SendNumLong(const std::ptrdiff_t& m_nNumber);

const XPlatform::Api::u32 m_RecvProto();
const std::ptrdiff_t m_RecvNumLong();

bool m_SendData(const XPlatform::Api::u32 m_nDataInfo, const char* Data, int len);

bool m_sSelect();

std::pair<bool, std::vector<std::pair<bool, std::string>>>m_LoadMessagesFromSession(const std::string YourNick, const std::string FriendNick);

bool m_sMessage(const std::string& m_sMsg, XPlatform::Api::u32 To);

std::string m_RecvMessage();

#endif