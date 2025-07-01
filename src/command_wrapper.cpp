/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class BasePanel                                                    *
* class LoopPanel                                                    *
* class CloseLoopPanel                                               *
* class CommandPanel                                                 *
* class InputCommandWrapper                                          *
* class ControlCommandWrapper                                        *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "command_wrapper.h"

#include "input_command.h"
#include "event_definitions.h"
#include "enumerations.h"

#include "dedicated_popups.h"

//====================================================================

extern wxIntegerValidator<unsigned int> s_integerValidator;

//====================================================================

LoopPanel::LoopPanel(wxWindow* parent, uint posY, uint width, int times)
:BaseWrapperPanel<BasePanel>(parent, posY, width)
{
	m_times=times;

	SetForegroundColour(wxColor(*wxBLUE));

	wxStaticText* loopDescription=new wxStaticText(this, wxID_ANY, wxT("Start Loop x: "));

	m_loopInput=new WX_TextCtrl(this, wxID_ANY, wxString::Format(wxT("%i"), times),
								wxDefaultPosition, wxSize(100, 23),  0, s_integerValidator);

	m_loopInput->setCallback([this](const char* val){
		m_times=wxAtoi(val);
	});

	wxBoxSizer* row=new wxBoxSizer(wxHORIZONTAL);
	row->Add(loopDescription, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(10));
	row->Add(m_loopInput, 0);
	
	wxBoxSizer* col=new wxBoxSizer(wxVERTICAL);
	col->Add(row, 1, wxALL, FromDIP(10));

	SetSizerAndFit(col);

	m_height=this->GetMinHeight();
	int w=parent->GetSize().GetWidth()-10;
	this->SetSize(w, m_height);
}

//====================================================================

CloseLoopPanel::CloseLoopPanel(wxWindow* parent, uint posY, uint width)
:BaseWrapperPanel<BasePanel>(parent, posY, width)
{
	SetForegroundColour(wxColor(*wxBLUE));

	wxStaticText* loopDescription=new wxStaticText(this, wxID_ANY, wxT("End Loop"));

	wxBoxSizer* row=new wxBoxSizer(wxHORIZONTAL);
	row->Add(loopDescription, 1, wxEXPAND);

	wxBoxSizer* col=new wxBoxSizer(wxVERTICAL);
	col->Add(row, 1, wxALL, FromDIP(10));

	SetSizerAndFit(col);

	m_height=this->GetMinHeight();
	int w=parent->GetSize().GetWidth()-10;
	this->SetSize(w, m_height);
}

//====================================================================
//====================================================================

BEGIN_EVENT_TABLE(CommandPanel, BasePanel)
	EVT_CHECKBOX(EvtID::ID, CommandPanel::OnCheck)
	EVT_LEFT_DOWN(CommandPanel::MouseLeftBtnDown)
	EVT_CONTEXT_MENU(CommandPanel::OnContextMenu)

	EVT_BUTTON(WX::CMD_STATUS, CommandPanel::OnCheckStatus)
	
END_EVENT_TABLE()

//--------------------------------------------------------------------

void CommandPanel::init(bool indentation)
{
	m_isIndented=indentation;

	m_enableCmdCheck=new wxCheckBox(m_handlerPtr, EvtID::ID, wxT(" "),
					wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	m_enableCmdCheck->SetValue(getCommand()->isActive());

	wxString description=wxString::FromUTF8(reinterpret_cast<const char*>(getCommand()->getDescription()));

	m_description = new WX_TextCtrl(m_handlerPtr, wxID_ANY, description, wxDefaultPosition,
	                      //wxDefaultSize,//
	                      wxSize(-1, 23),
	                      wxNO_BORDER);

	m_description->setCallback([this](const char* val){		
		getCommand()->updateDescription(reinterpret_cast<const char8_t*>(val));
	});

	setTimeoutCtrl();

	m_statusBtn=new wxButton(m_handlerPtr, WX::CMD_STATUS, wxT(""),
		wxDefaultPosition, wxSize(20,20), wxNO_BORDER | wxBU_EXACTFIT);

	m_statusBtn->Disable();
}

//--------------------------------------------------------------------

void CommandPanel::OnCheckStatus(wxCommandEvent& event)
{
	wxString msg=wxString::Format(wxT("Error: %s."), ExitCode::getExitCodeMsg(getCommand()->getExitCode()));
	wxMessageBox(msg);
}

//--------------------------------------------------------------------

void CommandPanel::enableCommand(bool enable)
{
	getCommand()->updateActive(enable);
	m_enableCmdCheck->SetValue(enable);
	if(enable){
		m_description->Enable();
		m_timeoutInput->Enable();		
	}
	else{
		m_description->Disable();
		m_timeoutInput->Disable();
	}
}

void CommandPanel::enableStatus()
{
	if(getCommand()->getExitCode()>0){
		m_statusBtn->Enable();
		m_statusBtn->SetBackgroundColour(wxColour("#FF0000"));
	}
	else{
		m_statusBtn->SetBackgroundColour(wxColour("#49d470"));
	}
}


//--------------------------------------------------------------------

void CommandPanel::OnCheck(wxCommandEvent& event)
{	
	enableCommand(m_enableCmdCheck->GetValue());
}

//--------------------------------------------------------------------

void CommandPanel::doIndentation(bool indentation)
{
	if(m_isIndented!=indentation){
		
		m_isIndented=indentation;

		int parentWidth=GetParent()->GetSize().GetWidth();

		m_mainCol->SetMinSize(parentWidth-2*CmdSettingData::MARGIN_WIDTH-(c_padding*indentation), 10);

		auto paddingColNew=new wxBoxSizer(wxHORIZONTAL);
		paddingColNew->Add(c_padding*indentation, 0, wxALL, 20);

		m_sizerBody->Replace(m_paddingCol, paddingColNew);
		m_paddingCol=paddingColNew;
		m_sizerBody->Layout();
	}
}

//====================================================================

static wxFloatingPointValidator<float>& initFloatValidator()
{
	static wxFloatingPointValidator<float> s_floatValidator;
	s_floatValidator.SetRange(0, 60*60*2);
	s_floatValidator.SetPrecision(2);
	return s_floatValidator;
}

wxFloatingPointValidator<float>& s_floatValidator=initFloatValidator();


InputCommandWrapper::InputCommandWrapper(wxWindow* parent, uint posY, uint width, InputCommand* cmd)
:CommandPanel(parent, posY, width)
, m_cmdPtr(cmd)
{
}

BEGIN_EVENT_TABLE(InputCommandWrapper, CommandPanel)
END_EVENT_TABLE()

//--------------------------------------------------------------------

InputCommandWrapper::~InputCommandWrapper()
{
	delete m_cmdPtr;
	m_cmdPtr=nullptr;
}

BaseCommand* InputCommandWrapper::getCommand()
{
	return m_cmdPtr;
}

//--------------------------------------------------------------------

void InputCommandWrapper::setTimeoutCtrl()
{
	m_timeoutInput = new WX_TextCtrl(m_handlerPtr, wxID_ANY, wxT("1"), wxDefaultPosition,
								wxSize(80, 23), wxNO_BORDER, s_floatValidator);

	m_timeoutInput->setCallback([this](const char* val){
		m_cmdPtr->updateTime(std::atof(val)*1000);
	});

	m_timeoutInput->Bind(wxEVT_TEXT, [this](wxCommandEvent& event) {
		wxCommandEvent event2(wxEVT_CUSTOM_EVENT, EvtID::CHANGES_MADE);
		wxPostEvent(this, event2);
   });
}

//--------------------------------------------------------------------

void InputCommandWrapper::init(bool indentation)
{
	CommandPanel::init(indentation);

	wxStaticText* paddingTime=new wxStaticText(m_handlerPtr, wxID_ANY, wxT("Wait for (secs): "));

	m_timeoutInput->ChangeValue(wxString::Format(wxT("%.2f"), m_cmdPtr->wait()/1000.0));

	//layout

	wxBoxSizer* row1=new wxBoxSizer(wxHORIZONTAL);
	
	row1->Add(m_enableCmdCheck, 0, wxALIGN_CENTER_VERTICAL);
	row1->Add(m_description, 1, wxALIGN_CENTER_VERTICAL);//, wxEXPAND | wxTOP, 50);

	int boxPlaceholder=c_padding+5;
	wxBoxSizer* row2 = new wxBoxSizer(wxHORIZONTAL);
	row2->Add(boxPlaceholder, 0);
	row2->Add(paddingTime, 0);
	row2->Add(m_timeoutInput, 0);
	row2->AddStretchSpacer();
	row2->Add(m_statusBtn, 0);

	m_mainCol= new wxBoxSizer(wxVERTICAL);
	m_mainCol->Add(row1, 1, wxEXPAND);
	m_mainCol->Add(row2, 0, wxEXPAND | wxTOP, 4);

	m_sizerBody = new wxBoxSizer(wxHORIZONTAL);

	m_paddingCol=new wxBoxSizer(wxHORIZONTAL);
	m_paddingCol->Add(c_padding*indentation, 0);

	m_sizerBody->Add(m_paddingCol, 0);
	m_sizerBody->Add(m_mainCol, 1, wxEXPAND);

	m_handlerPtr->SetSizerAndFit(m_sizerBody);

	auto sizerVert = new wxBoxSizer(wxHORIZONTAL);
	sizerVert->Add(m_handlerPtr, 1, wxEXPAND | wxTOP, CmdSettingData::TOP_MARGIN_PADDING);
	auto sizerHor = new wxBoxSizer(wxHORIZONTAL);
	sizerHor->Add(sizerVert, 1, wxEXPAND | wxLEFT|wxRIGHT | wxBOTTOM, CmdSettingData::MARGIN_WIDTH);
	
	this->SetSizerAndFit(sizerHor);
	m_height=this->GetMinHeight();

	int parentWidth=GetParent()->GetSize().GetWidth();

	this->SetSize(parentWidth-10, m_height);
}

//====================================================================


MouseBtnCmdWrapper::MouseBtnCmdWrapper(wxWindow* parent, uint posY, uint width, MouseBtnCommand* cmd)
:InputCommandWrapper(parent, posY, width, cmd)
{
}

BEGIN_EVENT_TABLE(MouseBtnCmdWrapper, InputCommandWrapper)
END_EVENT_TABLE()

//--------------------------------------------------------------------

void MouseBtnCmdWrapper::init(bool indentation)
{
	CommandPanel::init(indentation);

	wxStaticText* paddingTime=new wxStaticText(m_handlerPtr, wxID_ANY, wxT("Wait for (secs): "));

	m_timeoutInput->ChangeValue(wxString::Format(wxT("%.2f"), m_cmdPtr->wait()/1000.0));

	wxStaticText* pressForTxt=new wxStaticText(m_handlerPtr, wxID_ANY, wxT("Press for (ms): "));

	MouseBtnCommand* mouseCmdPtr=dynamic_cast<MouseBtnCommand*>(m_cmdPtr);
	
	m_pressForMsInput = new WX_TextCtrl(m_handlerPtr, wxID_ANY, wxT("0"), wxDefaultPosition,
								wxSize(70, 23), wxNO_BORDER, s_integerValidator);

	m_pressForMsInput->ChangeValue(wxString::Format(wxT("%d"), mouseCmdPtr->getPressFor()));

	m_pressForMsInput->setCallback([mouseCmdPtr](const char* val){
		mouseCmdPtr->setPressFor(std::atoi(val));
	});

	m_pressForMsInput->Bind(wxEVT_TEXT, [this](wxCommandEvent& event) {
		wxCommandEvent event2(wxEVT_CUSTOM_EVENT, EvtID::CHANGES_MADE);
		wxPostEvent(this, event2);
   });

	//layout

	wxBoxSizer* row1=new wxBoxSizer(wxHORIZONTAL);
	
	row1->Add(m_enableCmdCheck, 0, wxALIGN_CENTER_VERTICAL);
	row1->Add(m_description, 1, wxALIGN_CENTER_VERTICAL);//, wxEXPAND | wxTOP, 50);

	int boxPlaceholder=c_padding+5;
	wxBoxSizer* row2 = new wxBoxSizer(wxHORIZONTAL);
	row2->Add(boxPlaceholder, 0);
	row2->Add(paddingTime, 0);
	row2->Add(m_timeoutInput, 0);

	row2->Add(pressForTxt, 0, wxLEFT, 20);
	row2->Add(m_pressForMsInput, 0);
	row2->AddStretchSpacer();
	row2->Add(m_statusBtn, 0);

	m_mainCol= new wxBoxSizer(wxVERTICAL);
	m_mainCol->Add(row1, 1, wxEXPAND);
	m_mainCol->Add(row2, 0, wxEXPAND | wxTOP, 4);

	m_sizerBody = new wxBoxSizer(wxHORIZONTAL);

	m_paddingCol=new wxBoxSizer(wxHORIZONTAL);
	m_paddingCol->Add(c_padding*indentation, 0);

	m_sizerBody->Add(m_paddingCol, 0);
	m_sizerBody->Add(m_mainCol, 1, wxEXPAND);

	m_handlerPtr->SetSizerAndFit(m_sizerBody);

	auto sizerVert = new wxBoxSizer(wxHORIZONTAL);
	sizerVert->Add(m_handlerPtr, 1, wxEXPAND | wxTOP, CmdSettingData::TOP_MARGIN_PADDING);
	auto sizerHor = new wxBoxSizer(wxHORIZONTAL);
	sizerHor->Add(sizerVert, 1, wxEXPAND | wxLEFT|wxRIGHT | wxBOTTOM, CmdSettingData::MARGIN_WIDTH);
	
	this->SetSizerAndFit(sizerHor);
	m_height=this->GetMinHeight();

	int parentWidth=GetParent()->GetSize().GetWidth();

	this->SetSize(parentWidth-10, m_height);
}

//====================================================================

ControlCommandWrapper::ControlCommandWrapper(wxWindow* parent, uint posY, uint width, CtrlCommand* cmdPtr)
:CommandPanel(parent, posY, width)
, m_cmdPtr(cmdPtr)
, m_imgCtrlViewrPtr(nullptr)
{
	Bind(wxEVT_MENU, [this](wxCommandEvent& evnt){
		wxCommandEvent event(wxEVT_CUSTOM_EVENT, EvtID::EDIT_CTRL_CMD);
		event.SetClientData(m_cmdPtr);
		wxPostEvent(this, event);
	}, WX::CTRL_CMD_MENU_EDIT);

	cmdPtr->addHandler(HANDLERS::UPDATE_TIMEOUT, [this, cmdPtr](){
		updateTimeout(cmdPtr->getTimeout());
		Refresh();
	});

	cmdPtr->addHandler(HANDLERS::BLOCK_STATIC_BTN, [this](){
		m_statusBtn->Disable();
	});
	
}

//--------------------------------------------------------------------

ControlCommandWrapper::~ControlCommandWrapper()
{
	delete m_cmdPtr;
	m_cmdPtr=nullptr;
}

//--------------------------------------------------------------------

BEGIN_EVENT_TABLE(ControlCommandWrapper, CommandPanel)
	EVT_CHECKBOX(EvtID::ID, ControlCommandWrapper::OnCheck)
	EVT_CONTEXT_MENU(ControlCommandWrapper::OnContextMenu)
END_EVENT_TABLE()

//--------------------------------------------------------------------

BaseCommand* ControlCommandWrapper::getCommand()
{
	return m_cmdPtr;
}

//--------------------------------------------------------------------

void ControlCommandWrapper::mkContextMenu()
{
	if(m_isSelected){
		wxMenu menu;
		menu.Append(WX::CTRL_CMD_MENU_EDIT, wxT("Edit"));
		menu.Append(WX::DELETE_CMD, wxT("Delete"));
		popupMenu(this, &menu);
	}
}

//--------------------------------------------------------------------

void ControlCommandWrapper::OnCheckStatus(wxCommandEvent& event)
{
	if(ExitCode::FAILED<(m_cmdPtr->getExitCode())){
		wxString msg=wxString::Format(wxT("Error: %s."), ExitCode::getExitCodeMsg(m_cmdPtr->getExitCode()));
		wxMessageBox(msg);
	}
	else{
		if(!m_imgCtrlViewrPtr){
			m_imgCtrlViewrPtr=new ResultPopup(this, "Image for Ctrl Command", m_cmdPtr);
		}
		else{
			m_imgCtrlViewrPtr->loadBaseImg(m_cmdPtr->getBaseImg());
		}
		m_imgCtrlViewrPtr->Popup();
	}
}

//--------------------------------------------------------------------

void ControlCommandWrapper::setTimeoutCtrl()
{
	m_timeoutInput = new WX_TextCtrl(m_handlerPtr, wxID_ANY, wxT("1"), wxDefaultPosition,
								wxSize(80, 23), wxNO_BORDER, s_integerValidator);

	m_timeoutInput->setCallback([this](const char* val){
		m_cmdPtr->updateTime(std::atoi(val));
	});

	m_timeoutInput->Bind(wxEVT_TEXT, [this](wxCommandEvent& event) {
		wxCommandEvent event2(wxEVT_CUSTOM_EVENT, EvtID::CHANGES_MADE);
		wxPostEvent(this, event2);
   });
}

//--------------------------------------------------------------------

void ControlCommandWrapper::init(bool indentation)
{
	CommandPanel::init(indentation);	

	wxStaticText* timeoutText=new wxStaticText(m_handlerPtr, wxID_ANY, wxT("Timeout (secs): "));

	updateTimeout(m_cmdPtr->getTimeout());

	// Layout

	wxBoxSizer* row1=new wxBoxSizer(wxHORIZONTAL);
	row1->Add(m_enableCmdCheck, 0, wxALIGN_CENTER_VERTICAL);
	row1->Add(m_description, 1, wxALIGN_CENTER_VERTICAL);

	int boxPlaceholder=c_padding+5;
	wxBoxSizer* row2 = new wxBoxSizer(wxHORIZONTAL);
	row2->Add(boxPlaceholder, 0);
	row2->Add(timeoutText, 0);
	row2->Add(m_timeoutInput, 0);
	row2->AddStretchSpacer();
	//row2->Add(m_edit, 0, wxRIGHT, 5);
	row2->Add(m_statusBtn, 0);

	m_mainCol= new wxBoxSizer(wxVERTICAL);
	m_mainCol->Add(row1, 1, wxEXPAND);
	m_mainCol->Add(row2, 0, wxTOP | wxEXPAND, 4);

	m_paddingCol=new wxBoxSizer(wxHORIZONTAL);
	m_paddingCol->Add(c_padding*indentation, 0);

	m_sizerBody = new wxBoxSizer(wxHORIZONTAL);
	m_sizerBody->Add(m_paddingCol, 0);
	m_sizerBody->Add(m_mainCol, 1, wxEXPAND);

	m_handlerPtr->SetSizerAndFit(m_sizerBody);
	
	auto sizerVert = new wxBoxSizer(wxHORIZONTAL);
	sizerVert->Add(m_handlerPtr, 1, wxEXPAND | wxTOP, CmdSettingData::TOP_MARGIN_PADDING);
	auto sizerHor = new wxBoxSizer(wxHORIZONTAL);
	sizerHor->Add(sizerVert, 1, wxEXPAND | wxLEFT |wxRIGHT | wxBOTTOM, CmdSettingData::MARGIN_WIDTH);
	
	this->SetSizerAndFit(sizerHor);
	m_height=this->GetMinHeight();
	int parentWidth=GetParent()->GetSize().GetWidth();
	this->SetSize(parentWidth-10, m_height);
}

//====================================================================
