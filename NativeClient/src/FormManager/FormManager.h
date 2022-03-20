#ifndef _XMSG_FORM_MANAGER_H
#define _XMSG_FORM_MANAGER_H

#include "..//GUI/GUI.h"

#include<unordered_map>

namespace XMsg {
	namespace GUI {
		enum FormId {
			_XMSG_FORM_ID_NONE = 0,
			_XMSG_FORM_ID_LOGIN = 1,
			_XMSG_FORM_ID_CHAT = 2
		};

		class FormManager {
			std::unordered_map<XMsg::GUI::FormId, XMsg::GUI::Form*> m_hpForms;

			XMsg::GUI::Form* m_pCurrentForm;
			static FormManager* m_pInstance;
		public:

			FormManager();

			static FormManager* GetInstance();

			void SetCurrentForm(XMsg::GUI::FormId m_nID);

			XMsg::GUI::Form* GetForm(XMsg::GUI::FormId m_nID);

			XMsg::GUI::Form* GetCurrentForm();

			void Shutdown();
		};
	}
}

#endif