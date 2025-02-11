/*********************************************************************
* wxBoxSizer* BoxSizerH()                                            *
* wxBoxSizer* BoxSizerV()                                            *
* template<typename... Args> void wxMsgBox                           *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _WX_UTILS_H
#define _WX_UTILS_H

#include <wx/gdicmn.h> 
#include <wx/sizer.h>
#include <wx/string.h>

//====================================================================

inline wxBoxSizer* BoxSizerH()
{
	return new wxBoxSizer(wxHORIZONTAL);
}

inline wxBoxSizer* BoxSizerV()
{
	return new wxBoxSizer(wxVERTICAL);
}

template<typename... Args>
inline void wxMsgBox(const char* format, Args... args)
{
	wxString msg=wxString::Format(format, args...);
	wxMessageBox(msg);
};

//====================================================================

#endif
