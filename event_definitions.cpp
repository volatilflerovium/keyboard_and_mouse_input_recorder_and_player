/*********************************************************************
* Some custom event definitions                                      *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "event_definitions.h"

wxDEFINE_EVENT(wxEVT_CUSTOM_EVENT, wxCommandEvent);

wxDEFINE_EVENT(wxEVT_SELECTION_EVENT, wxCommandEvent);

wxDEFINE_EVENT(wxEVT_COMMAND_MYTHREAD_COMPLETED, wxThreadEvent);

wxDEFINE_EVENT(wxEVT_POPUP_DISAPPEARED, wxCommandEvent);					

wxDEFINE_EVENT(wxEVT_TXTCTRL_COMMAND, wxCommandEvent);
