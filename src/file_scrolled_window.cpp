/*********************************************************************
* class FilePanel                                                    *
* class FileScrolledWindow        	                                 *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "file_scrolled_window.h"
#include "utilities.h"
#include "debug_utils.h"

#include "wx_textctrl.h"
#include <filesystem>

//====================================================================

extern wxTextValidator s_fileValidator;

//====================================================================

FilePanel::FilePanel(wxWindow* parent, uint posY, uint width, const char* fileName, bool disable)
:WrapperPanel<FilePanel, 10, 2, WX::DELETE_FILE>(parent, posY, width)
, m_fileName(fileName)
{
	init(disable);
}

//--------------------------------------------------------------------

BEGIN_EVENT_TABLE(FilePanel, wxPanel)
	EVT_LEFT_DOWN(FilePanel::MouseLeftBtnDown)
	EVT_CONTEXT_MENU(FilePanel::OnContextMenu)
END_EVENT_TABLE()

//--------------------------------------------------------------------

void FilePanel::init(bool disable)
{
	int minWidth=(m_handlerPtr->GetSize()).GetWidth();

	m_fileNameInput= new WX_TextCtrl(m_handlerPtr, wxID_ANY, "", wxDefaultPosition,
								wxSize(minWidth, 30),
								wxNO_BORDER | wxTE_PROCESS_ENTER,
								s_fileValidator);

	if(disable){
		m_fileNameInput->Disable();
	}

	m_fileNameInput->ChangeValue(m_fileName);
	
	m_fileNameInput->setCallback([this](const char* val){
		
		if(m_fileName!=val && std::strlen(val)>0){
			std::error_code ec;
			std::string newFilePath=getFilePath(val);
			if(std::filesystem::exists(newFilePath, ec)){

				wxMessageDialog fileNameCheck(
					this,
					wxT("Filename already exists."),
					wxT(""),
					wxOK_DEFAULT|wxCENTRE|wxICON_WARNING
				);

				fileNameCheck.ShowModal();

				m_fileNameInput->ChangeValue(m_fileName);
				return;
			}
			std::filesystem::rename(getFilePath(m_fileName.mb_str()), newFilePath);
			m_fileName=val;
		}
	});
	/*
	m_fileNameInput->Bind(wxEVT_TEXT_ENTER, [this](wxCommandEvent& event){
	});
	*/

	wxBoxSizer* sizerBody = new wxBoxSizer(wxVERTICAL);
	sizerBody->Add(m_fileNameInput, 1, wxEXPAND);

	m_handlerPtr->SetSizerAndFit(sizerBody);

	auto sizerVert = new wxBoxSizer(wxVERTICAL);
	sizerVert->Add(m_handlerPtr, 1, wxEXPAND | wxTOP, _TOP_MARGIN_PADDING);
	auto sizerHor = new wxBoxSizer(wxHORIZONTAL);
	sizerHor->Add(sizerVert, 0, wxLEFT|wxBOTTOM, _MARGIN_WIDTH);
	
	this->SetSizerAndFit(sizerHor);
	m_height=this->GetMinHeight();
	int width=this->GetSize().GetWidth();

	this->SetSize(width, m_height);
}

//====================================================================
//====================================================================

FileScrolledWindow::FileScrolledWindow(wxWindow* parent, int Id, wxPoint Point, wxSize wSize)
:wxScrolledWindow(parent, Id, Point, wSize)
, m_width(wSize.GetWidth())
, m_height(0)
, m_init(false)
{
	SetBackgroundColour(wxColour("#FFFFFF"));
}

FileScrolledWindow::~FileScrolledWindow()
{
	for(FilePanel* panelPtr : m_filePanelList){
		wxDELETE(panelPtr);
	}
}

//--------------------------------------------------------------------

BEGIN_EVENT_TABLE(FileScrolledWindow, wxScrolledWindow)
	EVT_MENU(WX::DELETE_FILE, FileScrolledWindow::DeleteFile)
END_EVENT_TABLE()

//--------------------------------------------------------------------

void FileScrolledWindow::DeleteFile(wxCommandEvent& event)
{
	wxMessageDialog confirm(
		this,
		wxT("Do you want to delete the selected File?"),
		wxT(""),
		wxYES_NO|wxCENTRE|wxICON_WARNING
	);

	if(wxID_NO==confirm.ShowModal()){
		return;
	}

	if(m_filePanelList.size()>0){
		Scroll(0, 0);
		int height=0;
		int scrollPosition=0;
		int hPosition=0;
		bool found=false;
		for(size_t i=0; i<m_filePanelList.size(); i++){
			FilePanel* filePanelPtr= m_filePanelList[i];
			if(filePanelPtr->isSelected()){
				FilePanel::s_lastSelected=nullptr;
				height=-1*filePanelPtr->getHeight();

				std::error_code ec;
				std::filesystem::path filePath{getFilePath(filePanelPtr->getFileName().mb_str())};
				if(std::filesystem::exists(filePath, ec)){
					std::filesystem::remove(filePath);
					wxCommandEvent event(wxEVT_CUSTOM_EVENT, EvtID::REMOVE_FILE_FROM_DROPDOWN);
					event.SetString(filePanelPtr->getFileName());
					wxPostEvent(this, event);
				}

				found=true;
				scrollPosition=hPosition;
				delete(filePanelPtr);
				continue;
			}

			if(found){
				m_filePanelList[i-1]=m_filePanelList[i];
				filePanelPtr->SetPosition(wxPoint(0, hPosition));
			}
		
			hPosition+=filePanelPtr->getHeight();
		}

		updateScroll(height);
		Scroll(0, scrollPosition/10);

		m_filePanelList.pop_back();
	}
}

//--------------------------------------------------------------------

void FileScrolledWindow::updateScroll(int height)
{
	m_height+=height;

	const int stepY=10;
	int hy=1+std::floor(m_height/stepY);

	SetScrollbars(1, stepY, 0, hy, 0, 0, true);
	Scroll(0, hy);
}

//----------------------------------------------------------------------

void FileScrolledWindow::addFilePanel(const char* fileName, bool disable)
{
	Scroll(0, 0);// This is needed otherwise the panel is not position correctly
	FilePanel* filePanel=new FilePanel(this, m_height, m_width, fileName, disable);
	
	updateScroll(filePanel->getHeight());
	m_filePanelList.push_back(filePanel);
}

//--------------------------------------------------------------------

void FileScrolledWindow::clear()
{
	m_height=0;
	for(FilePanel* panelPtr : m_filePanelList){
		wxDELETE(panelPtr);
	}

	FilePanel::s_lastSelected=nullptr;

	m_filePanelList.clear();

	SetScrollbars(1, 1, 0, 0, 0, 0);
	Scroll(0, 0);
}

//====================================================================

