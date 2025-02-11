/*********************************************************************
* Some custom event definitions                                      *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef EVENT_DEFINITIONS_H
#define EVENT_DEFINITIONS_H

#include <wx/event.h>

wxDECLARE_EVENT(wxEVT_CUSTOM_EVENT, wxCommandEvent);

wxDECLARE_EVENT(wxEVT_SELECTION_EVENT, wxCommandEvent);

wxDECLARE_EVENT(wxEVT_POPUP_DISAPPEARED, wxCommandEvent);

wxDECLARE_EVENT(wxEVT_TXTCTRL_COMMAND, wxCommandEvent);

enum EvtID
{
	ID=2500,
	EDIT_CTRL_CMD,
	ADD_CTRL_CMD,
	UPDATE_CTRL_CMD,
	TXTCTRL_KILL_FOCUS,
	CHANGES_MADE,
	NEW_SELECTION,
	CHANGE_ROI_SCREENSHOT,
	UPDATE_CMD_VIEW,
	CMD_COUNT_UPDATED,
	REMOVE_FILE_FROM_DROPDOWN,
};

inline void postEvent(wxEvtHandler* h, wxEventType commandEventType, int id)
{
	wxCommandEvent event(commandEventType, id);
	wxPostEvent(h, event);
}

#endif
