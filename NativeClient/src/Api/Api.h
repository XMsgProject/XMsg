#ifndef _XMSG_API_H
#define _XMSG_API_H

#include <string>
#include <XPlatform.core/api.h>
#include <utility>
#include <XMsg.Api\XMsg.h>

namespace XMsg {
	namespace Net {
		typedef uint32_t _user_id_t;

		typedef void(*pfn_MessageCallback)(const std::string& Msg, const XPlatform::Api::u32 From);

		bool Init(const char* IP, bool IsHostname, bool IsIPv6, uint16_t Port);

		void SetMessageCallback(pfn_MessageCallback callback);

		bool Auth(const std::string& Login, const std::string& Pass);

		std::pair<bool, std::vector<std::string>> RequestFriendNames();
		std::pair<bool, std::vector<XMSG_API_TEXTURE_DATA>> RequestFriendIcons();
		std::pair<bool, std::vector<_user_id_t>> RequestFriendIDs();

		std::pair<bool, std::vector<std::pair<bool, std::string>>>LoadMessagesFromSession(const std::string YourNick, const std::string FriendNick);

		void x_SendMessage(const std::string& Message, const std::ptrdiff_t m_nSessionId);

		void Disconnect();

		void Listen();

		extern void (*Update)();
	}
}


#endif