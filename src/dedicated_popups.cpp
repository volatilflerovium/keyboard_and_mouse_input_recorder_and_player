/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class AddCmdPopup                                                  *
* class EditCtrlCmdPopup                                             *
* class WindowPreview                                                *
* class FileListPopup                                                *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "dedicated_popups.h"

#include "image_panel.h"
#include "event_definitions.h"
#include "input_command.h"
#include "enumerations.h"
#include "wx_textctrl.h"
#include "wxstring_array.h"

#include "file_scrolled_window.h"

#include <wx/valnum.h>
#include <wx/spinctrl.h>
#include <wx/window.h>

#include <filesystem>

#define MAX_PREVIEW_WIDTH 230
#define MAX_PREVIEW_HEIGHT 230


extern wxIntegerValidator<unsigned int> s_integerValidator;

//====================================================================

AddCmdPopup::AddCmdPopup(wxWindow* parent, const char* title)
:ExtendedPopup(parent, title)
{
	init();
	layout();
	setOnDismissCallback([this](wxWindow*){
		removeImg();
	});
}

//--------------------------------------------------------------------

BEGIN_EVENT_TABLE(AddCmdPopup, ExtendedPopup)
	EVT_BUTTON(WX::ADD_CTRL_CMD, AddCmdPopup::OnAddCtrlCmd)
END_EVENT_TABLE()

//--------------------------------------------------------------------

void AddCmdPopup::init()
{
	m_instructions=new wxStaticText(this, wxID_ANY,
								wxT("Wait for an image in the same region to be:"));

	ArrayStringType choices(2,"");
	choices[CTRL_CMD_MODE::SIMILAR]="Similar";
	choices[CTRL_CMD_MODE::DIFFERENT]="Different";

	m_ctrlCmdModeSetRadio = new wxRadioBox(this, wxID_ANY, "",
					wxDefaultPosition, wxDefaultSize, choices, 2, wxRA_VERTICAL);
	m_ctrlCmdModeSetRadio->SetSelection(0);

	m_timeoutTxt=new wxStaticText(this, wxID_ANY, wxT("Timeout (secs):"));

	m_timeoutInput=new wxSpinCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition,
										wxDefaultSize, wxSP_ARROW_KEYS, 0, 360000, 5);


	m_cancelBtn = new wxButton(this, wxID_ANY, wxT("Cancel"));
	m_cancelBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
		onCancel();
	});

	m_summitBtn = new wxButton(this, WX::ADD_CTRL_CMD, wxT("OK"));

	m_previewTxt=new wxStaticText(this, wxID_ANY, wxT("Preview"));

	m_previewPanel=new ImagePanel(this, wxID_ANY, wxDefaultPosition,
									FromDIP(wxSize(MAX_PREVIEW_WIDTH, MAX_PREVIEW_HEIGHT)));

	m_thresholdTxt=new wxStaticText(this, wxID_ANY, wxT("Image simililarity index:"));

	m_thresholdInput=new wxSpinCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition,
										wxDefaultSize, wxSP_ARROW_KEYS, 0, 441, 240);

	m_strictRunCheck=builder<wxCheckBox>(wxID_ANY, wxT("Terminate session on failure"));

	/*
	("The less the more restrictive. Max 440.\nSee the documentation for more info."));
	// */
}

//--------------------------------------------------------------------

void AddCmdPopup::layout()
{
	wxBoxSizer* leftCol = new wxBoxSizer(wxVERTICAL);
	leftCol->Add(m_previewTxt, 0, wxALIGN_TOP | wxBOTTOM, FromDIP(10));

	wxBoxSizer* leftRowCol = new wxBoxSizer(wxHORIZONTAL);
	leftRowCol->SetMinSize(FromDIP(wxSize(MAX_PREVIEW_WIDTH, 10)));
	leftRowCol->Add(m_previewPanel, 0, wxALIGN_CENTER_VERTICAL);
	leftCol->Add(leftRowCol, 1,  wxCENTER);

	wxBoxSizer* bodyRow = new wxBoxSizer(wxHORIZONTAL);
	bodyRow->Add(leftCol, 1, wxRIGHT | wxEXPAND, FromDIP(10));
	
	wxBoxSizer* rightCol = new wxBoxSizer(wxVERTICAL);
	rightCol->Add(m_instructions, 0, wxBOTTOM, FromDIP(5));
	rightCol->Add(m_ctrlCmdModeSetRadio, 0, wxCENTER | wxBOTTOM, FromDIP(10));

	wxBoxSizer* thresholdRow = new wxBoxSizer(wxHORIZONTAL);
	thresholdRow->Add(m_thresholdTxt, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(5));
	thresholdRow->Add(m_thresholdInput, 0);
	rightCol->Add(thresholdRow, 0, wxBOTTOM, FromDIP(10));
	
	wxBoxSizer* timeoutRow = new wxBoxSizer(wxHORIZONTAL);
	timeoutRow->Add(m_timeoutTxt, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(5));
	timeoutRow->Add(m_timeoutInput, 0);
	rightCol->Add(timeoutRow, 0, wxBOTTOM, FromDIP(10));

	rightCol->Add(m_strictRunCheck, 0, wxBOTTOM, FromDIP(10));

	wxBoxSizer* buttonRow = new wxBoxSizer(wxHORIZONTAL);
	buttonRow->Add(m_cancelBtn, 0, wxRIGHT, FromDIP(10));
	buttonRow->Add(m_summitBtn, 0);		
	rightCol->Add(buttonRow, 0, wxALIGN_RIGHT);

	bodyRow->Add(rightCol, 0, wxEXPAND);

	setSizer(bodyRow);
}

//--------------------------------------------------------------------

void AddCmdPopup::removeImg()
{
	if(m_imgName.length()>0){
		std::error_code ec;
		std::filesystem::path imgPath{getImgPath(m_imgName)};
		if(std::filesystem::exists(imgPath, ec)){
			std::filesystem::remove(imgPath);
			m_imgName="";
		}
	}
}

//--------------------------------------------------------------------

void AddCmdPopup::loadRoi(const char* imgName, const char* roi, const char* windowName)
{
	m_imgName=imgName;
	m_roi=roi;
	m_windowName=windowName;
	m_previewPanel->loadBackground(getImgPath(imgName).c_str());
	Layout();
}

//--------------------------------------------------------------------

void AddCmdPopup::OnAddCtrlCmd(wxCommandEvent& event)
{
	if(WX::ADD_CTRL_CMD==event.GetId()){
		Dismiss();
		
		CtrlCommand* commandPtr=CtrlCommand::Builder("Watch ROI",
						m_imgName, m_roi.c_str(), m_windowName.c_str(), 240, 100);

		int timeout=m_timeoutInput->GetValue();
		commandPtr->updateTime(timeout);

		bool compareSimilar=(CTRL_CMD_MODE::SIMILAR==m_ctrlCmdModeSetRadio->GetSelection());
		commandPtr->setSimilarity(compareSimilar);

		int thres=m_thresholdInput->GetValue();
		commandPtr->setThreshold(thres);

		commandPtr->setRestriction(m_strictRunCheck->GetValue());

		wxCommandEvent event(wxEVT_CUSTOM_EVENT, EvtID::ADD_CTRL_CMD);
		event.SetClientData(commandPtr);
		wxPostEvent(this, event);
		m_imgName="";
	}
}

//====================================================================

EditCtrlCmdPopup::EditCtrlCmdPopup(wxWindow* parent, const char* title)
:AddCmdPopup(parent, title, true)
, m_ctrlCmdPtr(nullptr)
{
	init();
	layout();
	setOnDismissCallback([this](wxWindow*){
		removeImg();
		m_ctrlCmdPtr=nullptr;
	});

	setOnClose([this](){
		removeImg();
		m_ctrlCmdPtr=nullptr;
	});
}

//--------------------------------------------------------------------

BEGIN_EVENT_TABLE(EditCtrlCmdPopup, AddCmdPopup)
	EVT_BUTTON(WX::UPDATE_CMD, EditCtrlCmdPopup::OnUpdateCmd)
	EVT_RADIOBOX(WX::SWAP_SCREENSHOT, EditCtrlCmdPopup::OnSwapScreenshot)
END_EVENT_TABLE()

//--------------------------------------------------------------------

void EditCtrlCmdPopup::init()
{
	AddCmdPopup::init();

	m_updateScreenshotBtn = new wxButton(this, wxID_ANY, wxT("Change Screenshot"));

	m_updateScreenshotBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){

		wxCommandEvent event2(wxEVT_CUSTOM_EVENT, EvtID::CHANGE_ROI_SCREENSHOT);
		event2.SetString(m_ctrlCmdPtr->getWindowName());
		wxPostEvent(this, event2);
	});

	wxDELETE(m_summitBtn);// this was set in  AddCmdPopup::init() we need to set new id
	m_summitBtn = new wxButton(this, WX::UPDATE_CMD, wxT("Update"));

	ArrayStringType choicesSreen(2,"");
	choicesSreen[USE_ROI::OLD]="Use current";
	choicesSreen[USE_ROI::NEW]="Use new";

	m_swapScreenshotRadio = new wxRadioBox(this, WX::SWAP_SCREENSHOT, "", wxDefaultPosition,
											wxDefaultSize, choicesSreen, 2, wxRA_HORIZONTAL);

	m_swapScreenshotRadio->SetSelection(0);
	m_swapScreenshotRadio->Disable();
}

//--------------------------------------------------------------------

void EditCtrlCmdPopup::layout()
{
	wxBoxSizer* leftCol = new wxBoxSizer(wxVERTICAL);
	leftCol->SetMinSize(FromDIP(wxSize(MAX_PREVIEW_WIDTH, 10)));
	leftCol->Add(m_previewTxt, 0, wxBOTTOM, FromDIP(10));

	wxBoxSizer* imageRow=new wxBoxSizer(wxHORIZONTAL);
	imageRow->Add(m_previewPanel, 1, wxALIGN_CENTER_VERTICAL);
	leftCol->Add(imageRow , 1, wxCENTER | wxBOTTOM, FromDIP(10));

	leftCol->Add(m_updateScreenshotBtn, 0, wxBOTTOM | wxCENTER, FromDIP(3));
	leftCol->Add(m_swapScreenshotRadio, 0, wxCENTER);

	wxBoxSizer* rightCol = new wxBoxSizer(wxVERTICAL);
	rightCol->Add(m_instructions, 0);
	rightCol->Add(m_ctrlCmdModeSetRadio, 0,  wxCENTER | wxBOTTOM, FromDIP(10));

	wxBoxSizer* thresholdRow = new wxBoxSizer(wxHORIZONTAL);
	thresholdRow->Add(m_thresholdTxt, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(5));
	thresholdRow->Add(m_thresholdInput, 0);
	rightCol->Add(thresholdRow, 0, wxBOTTOM, FromDIP(10));
	
	wxBoxSizer* timeoutRow = new wxBoxSizer(wxHORIZONTAL);
	timeoutRow->Add(m_timeoutTxt, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(5));
	timeoutRow->Add(m_timeoutInput, 0);
	rightCol->Add(timeoutRow, 0, wxBOTTOM, FromDIP(10));

	rightCol->Add(m_strictRunCheck, 0, wxBOTTOM, FromDIP(10));

	wxBoxSizer* buttonRow = new wxBoxSizer(wxHORIZONTAL);
	buttonRow->Add(m_cancelBtn, 0, wxRIGHT, FromDIP(10));
	buttonRow->Add(m_summitBtn, 0);
	rightCol->Add(buttonRow, 0, wxALIGN_RIGHT);

	wxBoxSizer* bodyRow = new wxBoxSizer(wxHORIZONTAL);
	bodyRow->Add(leftCol, 1, wxRIGHT | wxEXPAND, FromDIP(10));
	bodyRow->Add(rightCol, 1, wxEXPAND);

	setSizer(bodyRow);
}

//--------------------------------------------------------------------

void EditCtrlCmdPopup::OnSwapScreenshot(wxCommandEvent& event)
{
	if(m_ctrlCmdPtr){
		std::string imgPath;
		if(m_swapScreenshotRadio->GetSelection()==1){
			imgPath=getImgPath(m_imgName);
		}
		else{
			imgPath=getImgPath(m_ctrlCmdPtr->getBaseImg());
		}

		m_previewPanel->loadBackground(imgPath.c_str());
		Layout();
	}
}

//--------------------------------------------------------------------

bool EditCtrlCmdPopup::loadCommand(CtrlCommand* ctrlCmdPtr)
{
	m_swapScreenshotRadio->SetSelection(0);
	m_swapScreenshotRadio->Disable();
	m_ctrlCmdPtr=ctrlCmdPtr;
	if(m_ctrlCmdPtr){
		m_updateScreenshotBtn->Enable();
		m_ctrlCmdModeSetRadio->Enable();

		CTRL_CMD_MODE similarity=m_ctrlCmdPtr->getSimilarity();
		m_ctrlCmdModeSetRadio->SetSelection(similarity);

		if(imageExists(m_ctrlCmdPtr->getBaseImg())){
			m_previewPanel->loadBackground(getImgPath(m_ctrlCmdPtr->getBaseImg()).c_str(), wxBITMAP_TYPE_PNG);
		}
		else{
			std::string imgNotFound=resourcePath("icons/image_not_found.png");
			m_previewPanel->loadBackground(imgNotFound.c_str(), wxBITMAP_TYPE_PNG);
		}

		m_timeoutInput->SetValue(m_ctrlCmdPtr->getTimeout());

		m_thresholdInput->SetValue(m_ctrlCmdPtr->getThreshold());
		m_strictRunCheck->SetValue(m_ctrlCmdPtr->getRestriction());

		Layout();
		return true;
	}

	m_ctrlCmdModeSetRadio->Disable();

	return false;
}

//--------------------------------------------------------------------

void EditCtrlCmdPopup::OnUpdateCmd(wxCommandEvent& event)
{
	if(!m_ctrlCmdPtr){
		return;
	}

	bool similarity=CTRL_CMD_MODE::SIMILAR==static_cast<CTRL_CMD_MODE>(m_ctrlCmdModeSetRadio->GetSelection());
	
	m_ctrlCmdPtr->setSimilarity(similarity);			

	if(m_swapScreenshotRadio->GetSelection()==USE_ROI::NEW){
		m_ctrlCmdPtr->updateBaseImg(m_imgName.c_str(), m_roi.c_str());

		m_swapScreenshotRadio->SetSelection(USE_ROI::OLD);
		m_swapScreenshotRadio->Disable();
		m_imgName="";
	}

	m_ctrlCmdPtr->updateTime(m_timeoutInput->GetValue());

	uint thres=m_thresholdInput->GetValue();
	m_ctrlCmdPtr->setThreshold(thres);

	m_ctrlCmdPtr->setRestriction(m_strictRunCheck->GetValue());

	wxCommandEvent updateViewEvent(wxEVT_CUSTOM_EVENT, EvtID::UPDATE_CMD_VIEW);
	updateViewEvent.SetClientData(m_ctrlCmdPtr);
	wxPostEvent(this, updateViewEvent);

	m_ctrlCmdPtr=nullptr;
	Dismiss();

	wxCommandEvent event2(wxEVT_CUSTOM_EVENT, EvtID::CHANGES_MADE);
	wxPostEvent(this, event2);
}

//====================================================================

WindowPreview::WindowPreview(wxWindow* parent, const char* title)
:BasicPopup(parent, title)
, m_imageHolder(nullptr)
{
	m_cancelBtn = builder<wxButton>(wxID_ANY, wxT("Cancel"));
	m_okBtn = builder<wxButton>(WX::ADD_CTRL_CMD, wxT("OK"));

	m_previewPanel=builder<ImagePanel>(wxID_ANY, wxDefaultPosition, FromDIP(wxSize(350, 350)));

	m_btnsRow=new wxBoxSizer(wxHORIZONTAL);
	m_btnsRow->Add(m_cancelBtn, 0, wxRIGHT, FromDIP(10));
	m_btnsRow->Add(m_okBtn);
}

//--------------------------------------------------------------------

void WindowPreview::loadImage(const char* imageName)
{
	m_previewPanel->loadBackground(getImgPath(imageName).c_str(), wxBITMAP_TYPE_PNG);

	if(m_imageHolder){
		m_imageHolder->Detach(m_previewPanel);
		m_bodySizer->Detach(m_imageHolder);
		m_bodySizer->Detach(m_btnsRow);
	}

	auto tmpImageHolder=new wxBoxSizer(wxHORIZONTAL);
	tmpImageHolder->Add(m_previewPanel, 1, wxALIGN_CENTER);
	
	auto tmpBodySizer=new wxBoxSizer(wxVERTICAL);
	tmpBodySizer->Add(tmpImageHolder, 0, wxBOTTOM | wxALIGN_CENTER, FromDIP(10));
	tmpBodySizer->Add(m_btnsRow, 0, wxALIGN_RIGHT);

	setSizer(tmpBodySizer);
	
	m_bodySizer=tmpBodySizer;
	m_imageHolder=tmpImageHolder;
}

//====================================================================

FileListPopup::FileListPopup(wxWindow* parent, const char* title)
:WX_Popup(parent, title)
{
	m_fileScrolledWindow=new FileScrolledWindow(this, wxID_ANY, wxDefaultPosition,
	FromDIP(wxSize(450, 550)));
	wxBoxSizer* row=new wxBoxSizer(wxHORIZONTAL);
	row->Add(m_fileScrolledWindow, 1, wxEXPAND);
	wxBoxSizer* col=new wxBoxSizer(wxVERTICAL);
	col->Add(row, 1, wxEXPAND, FromDIP(10));
	this->SetSizerAndFit(col);

	setOnClose([this](){
		m_fileScrolledWindow->clear();
	});	
}

//====================================================================

void FileListPopup::OnDismiss()
{
	m_fileScrolledWindow->clear();
}

//--------------------------------------------------------------------

void FileListPopup::OnPopup()
{
	std::error_code ec;	
	std::string fileName;
	std::vector<std::string> commandFiles;
	std::filesystem::directory_iterator dirIterator(getFilePath().c_str(), ec);

	if(ec.value()==0){
		for(auto const& dirEntry : dirIterator){
			if(dirEntry.is_regular_file()){
				fileName=dirEntry.path().filename().c_str();
				if(fileName[0]!='.'){
					commandFiles.emplace_back(fileName.c_str());
				}
			}
		}
	}

	std::sort(commandFiles.begin(), commandFiles.end());
	for(auto& file : commandFiles){
		m_fileScrolledWindow->addFilePanel(file.c_str(), m_openFile==file);
	}
}

//====================================================================
