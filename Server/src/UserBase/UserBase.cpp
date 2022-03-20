#include "UserBase.h"

std::ptrdiff_t XMsg::Server::UserBase::NoUserId = PTRDIFF_MAX;

void XMsg::Server::UserBase::RegisterRootUser(const std::string& m_sLogin, const std::string& m_sPassword){
	UserBaseData RootBaseSector;

	RootBaseSector.m_sLogin = m_sLogin;
	RootBaseSector.m_sPassword = m_sPassword;
	RootBaseSector.m_nPrivilageLevel = 0x9000;

	m_vUserBase.push_back(RootBaseSector);
}

void XMsg::Server::UserBase::RegisterUser(const std::string& m_sLogin, const std::string& m_sPassword){
	UserBaseData NewBaseSector;

	NewBaseSector.m_sLogin = m_sLogin;
	NewBaseSector.m_sPassword = m_sPassword;
	NewBaseSector.m_nPrivilageLevel = 0x9001;

	m_vUserBase.push_back(NewBaseSector);
}

void XMsg::Server::UserBase::Disconnect(const std::ptrdiff_t& m_nBaseUserId){
	m_vUserBase[m_nBaseUserId].m_bIsAuthificate = false;
}

bool XMsg::Server::UserBase::AuthificateUser(const std::string& m_sLogin, const std::string& m_sPassword){

	for (UserBaseData& BaseSector : m_vUserBase) {
		if (BaseSector.m_sLogin == m_sLogin) {
			if (BaseSector.m_sPassword == m_sPassword)
				BaseSector.m_bIsAuthificate = true;
				return true;
			break;
		}
	}

	return false;
}

void XMsg::Server::UserBase::AddFriend(const std::ptrdiff_t& m_nBaseUserId, const std::ptrdiff_t& m_nAccountId){
	m_vUserBase[m_nBaseUserId].m_vnFriendAccountsId.push_back(m_nAccountId);
	m_vUserBase[m_nAccountId].m_vnFriendAccountsId.push_back(m_nBaseUserId);

	Session session;
	session.FirstUser = this->GetAccountLogin(m_nBaseUserId);
	session.FirstUserId = m_nBaseUserId;
	session.SecondUser = this->GetAccountLogin(m_nAccountId);
	session.SecondUserId = m_nAccountId;
	session.m_vIsFirstUserMessage.push_back(true);
	session.m_vMessages.push_back("[XMsg][Server]: Session created!");
	
	m_vSessions.push_back(session);
}

std::vector<std::ptrdiff_t>& XMsg::Server::UserBase::GetFriends(const std::ptrdiff_t& m_nBaseUserId){
	return m_vUserBase[m_nBaseUserId].m_vnFriendAccountsId;
}

std::string XMsg::Server::UserBase::GetAccountLogin(const std::ptrdiff_t& m_nBaseUserId){
	return m_vUserBase[m_nBaseUserId].m_sLogin;
}

std::string XMsg::Server::UserBase::GetUserIconPath(const std::ptrdiff_t& m_nBaseUserId){
	return m_vUserBase[m_nBaseUserId].m_sIconPath;
}

XMsg::Server::Session* XMsg::Server::UserBase::GetSession(const std::string& m_sFirstUser, const std::string& m_sSecondUser) {
	for (XMsg::Server::Session& m_vSession : m_vSessions) {
		if (
			(m_sFirstUser == m_vSession.FirstUser && m_sSecondUser == m_vSession.SecondUser)
			|| 
			(m_sSecondUser == m_vSession.FirstUser && m_sFirstUser == m_vSession.SecondUser)
			) {

			return &m_vSession;
		}
	}

	return NULL;
}

std::ptrdiff_t XMsg::Server::UserBase::GetUserId(const std::string& m_sLogin){

	for (std::ptrdiff_t i = 0; i < m_vUserBase.size(); i++) {
		if (m_vUserBase[i].m_sLogin == m_sLogin)
			return i;
	}

	return XMsg::Server::UserBase::NoUserId;
}
