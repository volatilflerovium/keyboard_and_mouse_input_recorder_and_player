/*********************************************************************
* class WX_TextCtrl                                                  *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _WX_TEXTCTRL_H
#define _WX_TEXTCTRL_H

#include "event_definitions.h"

#include <iostream>

#include <wx/wx.h>

//====================================================================

class WX_TextCtrl : public wxTextCtrl
{
	public:
		template<typename W, typename... Args>
		WX_TextCtrl(W* parent, Args... args)
		:wxTextCtrl(parent, args...)
		, m_cbk(nullptr)
		, m_enable(false)
		{
		}

		virtual ~WX_TextCtrl()=default;

		void setCallback(std::function<void(const char*)> cbk)
		{
			m_cbk=cbk;
		}

	private:
		std::function<void(const char*)> m_cbk;
		bool m_enable;

		void inputHander();
		void MouseRightBtnDown(wxMouseEvent& event);
		void MouseLeftBtnDown(wxMouseEvent& event);
		
		void OnFocus(wxFocusEvent& event);
		void RemoveFocus(wxCommandEvent& event);

		void OnKeyDown(wxKeyEvent& event);

		DECLARE_EVENT_TABLE()
};

//--------------------------------------------------------------------

inline void WX_TextCtrl::MouseRightBtnDown(wxMouseEvent& event)
{
	inputHander();
}

//--------------------------------------------------------------------

inline void WX_TextCtrl::MouseLeftBtnDown(wxMouseEvent& event)
{
	if(m_enable){
		event.Skip();
	}
}

//--------------------------------------------------------------------

inline void WX_TextCtrl::OnFocus(wxFocusEvent& event)
{
	event.Skip();// needed for display cursor
	wxCommandEvent event2(wxEVT_TXTCTRL_COMMAND);
	event2.SetId(EvtID::TXTCTRL_KILL_FOCUS);
	AddPendingEvent(event2);
}

//--------------------------------------------------------------------

inline void WX_TextCtrl::RemoveFocus(wxCommandEvent& event)
{
	if(!m_enable){
		Disable();
		Enable();
	}
}

//--------------------------------------------------------------------

inline void WX_TextCtrl::OnKeyDown(wxKeyEvent& event)
{
	if(event.GetKeyCode()==WXK_RETURN){
		inputHander();
		return;
	}	
	event.Skip();
}

//--------------------------------------------------------------------

#endif
