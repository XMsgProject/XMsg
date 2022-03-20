#include "FormManager.h"

#include "..//LoginForm/LoginForm.h"
#include "..//ChatForm/ChatForm.h"

XMsg::GUI::FormManager* XMsg::GUI::FormManager::m_pInstance = NULL;

XMsg::GUI::FormManager::FormManager(){
	m_pInstance = this;

	m_hpForms[XMsg::GUI::FormId::_XMSG_FORM_ID_LOGIN] = new XMsg::GUI::LoginForm;
	m_hpForms[XMsg::GUI::FormId::_XMSG_FORM_ID_CHAT] = new XMsg::GUI::ChatForm;
}

XMsg::GUI::FormManager* XMsg::GUI::FormManager::GetInstance(){
	return m_pInstance;
}

void XMsg::GUI::FormManager::SetCurrentForm(XMsg::GUI::FormId m_nID){
	m_pCurrentForm = m_hpForms[m_nID];
	m_pCurrentForm->SelectEvent();
}

XMsg::GUI::Form* XMsg::GUI::FormManager::GetForm(XMsg::GUI::FormId m_nID){
	return m_hpForms[m_nID];
}

XMsg::GUI::Form* XMsg::GUI::FormManager::GetCurrentForm(){
	return m_pCurrentForm;
}

void XMsg::GUI::FormManager::Shutdown(){

	delete reinterpret_cast<XMsg::GUI::LoginForm*>(m_hpForms[XMsg::GUI::FormId::_XMSG_FORM_ID_LOGIN]);
	delete reinterpret_cast<XMsg::GUI::ChatForm*>(m_hpForms[XMsg::GUI::FormId::_XMSG_FORM_ID_CHAT]);
	
}
