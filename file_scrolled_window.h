/*********************************************************************
* class FilePanel                                                    *
* class FileScrolledWindow        	                                 *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _FILE_SCROLLED_WINDOW_H
#define _FILE_SCROLLED_WINDOW_H

#include "wrapper_panel.h"

#include <wx/scrolwin.h>

#include <vector>

//====================================================================

class WX_TextCtrl;

class FilePanel : public WrapperPanel<FilePanel, 10, 2, WX::DELETE_FILE>
{
	public:
		FilePanel(wxWindow* parent, uint posY, uint width, const char* fileName, bool disable);

		virtual ~FilePanel()=default;

		virtual void init(bool disable);

		wxString getFileName() const
		{
			return m_fileName;
		}

	private:
		wxString m_fileName;
		WX_TextCtrl* m_fileNameInput;

		virtual void setSelected() override
		{
			s_lastSelected=this;
		}

		DECLARE_EVENT_TABLE()

		friend class FileScrolledWindow;
};

//====================================================================
//====================================================================

class FileScrolledWindow : public wxScrolledWindow
{
	public:
		FileScrolledWindow(wxWindow* parent, int Id, wxPoint Point, wxSize wSize);

		virtual ~FileScrolledWindow();

		void addFilePanel(const char* fileName, bool disable);
		size_t size() const;
		void clear();

	private:
		std::vector<FilePanel*> m_filePanelList;
		
		const int m_width;
		int m_height;
		bool m_init;

		void updateScroll(int height);

		void DeleteFile(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
};

//====================================================================

#endif
