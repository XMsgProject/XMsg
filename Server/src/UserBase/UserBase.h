#ifndef _XMSG_SERVER_USER_BASE_H
#define _XMSG_SERVER_USER_BASE_H

#include"..//Api/Api.h"

namespace XMsg {
	namespace Server {
		struct UserBaseData {
			std::string m_sLogin;
			std::string m_sPassword;
			std::vector<std::ptrdiff_t> m_vnFriendAccountsId;

			std::string m_sIconPath = "t.txt";


			bool m_bIsAuthificate = false;

			std::uint16_t m_nPrivilageLevel = 0;
		};

		struct Session {
			std::string FirstUser;
			std::ptrdiff_t FirstUserId;
			std::string SecondUser;
			std::ptrdiff_t SecondUserId;

			std::vector<std::string> m_vMessages;
			std::vector<bool>m_vIsFirstUserMessage;
		};

		class UserBase {
			
			std::vector<UserBaseData> m_vUserBase;

			std::vector<Session> m_vSessions;

		public:

			void RegisterRootUser(const std::string& m_sLogin, const std::string& m_sPassword);

			void RegisterUser(const std::string& m_sLogin, const std::string& m_sPassword);
			std::ptrdiff_t GetUserId(const std::string& m_sLogin);
			static std::ptrdiff_t NoUserId;

			void Disconnect(const std::ptrdiff_t& m_nBaseUserId);
			std::vector<std::ptrdiff_t>& GetFriends(const std::ptrdiff_t& m_nBaseUserId);

			std::string GetUserIconPath(const std::ptrdiff_t& m_nBaseUserId);

			void AddFriend(const std::ptrdiff_t& m_nBaseUserId, const std::ptrdiff_t& m_nFriendAccountId);

			std::string GetAccountLogin(const std::ptrdiff_t& m_nBaseUserId);

			Session* GetSession(const std::string& m_sFirstUser, const std::string& m_sSecondUser);

			bool AuthificateUser(const std::string& m_sLogin, const std::string& m_sPassword);

		};
	}
}

#endif