#ifndef _INC_XMSG_API_H
#define _INC_XMSG_API_H

#define XMSG_API_PROTOCOL_ID_CONNECT 1
#define XMSG_API_PROTOCOL_ID_DISCONNECT 2
#define XMSG_API_PROTOCOL_ID_LOGIN 3
#define XMSG_API_PROTOCOL_ID_AUTH_STATUS 4
#define XMSG_API_PROTOCOL_ID_MSG 5
#define XMSG_API_PROTOCOL_ID_CHECK 6
#define XMSG_API_PROTOCOL_ID_GET_FRIENDS_NAMES 7

#define XMSG_API_DATA_TYPE_ID_LOGIN 1
#define XMSG_API_DATA_TYPE_ID_PASSWORD 2
#define XMSG_API_DATA_TYPE_ID_MESSAGE 3
#define XMSG_API_DATA_TYPE_ID_NUM 4

#define XMSG_GET_API_INTERFACE_NAME "XMsgApiGetInterface"
#define XMSG_GET_API_EXT_NAME "XMSG_API_EXT"

#include <XPlatform.net/Net.h>

struct XMSG_API_TEXTURE_DATA {
	int Width;
	int Height;
	int NrChannels;
	std::vector<unsigned char> Data;
	std::string FileName;
};

typedef uint32_t _user_id_t;

struct XMSG_API_INTERFACE {
	XPlatform::Net::XPlatformNetResult(*ConnectToServer)
		(const std::string& m_sIP, bool m_bIsHostname, bool m_bIPv6, XPlatform::Api::u16 m_nPort);
	
	void (*Disconnect)();

	bool (*SendProto)(const XPlatform::Api::u32 m_nProtoId);
	bool (*SendData)(const XPlatform::Api::u32 m_nDataInfo, const char* Data, int len);
	bool (*SendNumLong)(const std::ptrdiff_t& m_nNum);

	bool (*Login)(const std::string& m_sLogin, const std::string& m_sPass);

	std::pair<bool, std::vector<std::pair<bool, std::string>>>(*LoadMessagesFromSession)(const std::string YourNick, const std::string FriendNick);

	bool (*Message)(const std::string& m_sMsg, XPlatform::Api::u32 To);
	
	bool (*Select)();

	const std::pair<bool, std::vector<std::string>>(*RequestFriendNames)();
	const std::pair<bool, std::vector<_user_id_t>>(*RequestFriendIds)();
	std::pair<bool, std::vector<XMSG_API_TEXTURE_DATA>>(*RequestFriendIcons)();

	std::string (*RecvMessage)();
	const XPlatform::Api::u32(*RecvNum)();

	const XPlatform::Api::u32(*RecvProto)();

};

#endif