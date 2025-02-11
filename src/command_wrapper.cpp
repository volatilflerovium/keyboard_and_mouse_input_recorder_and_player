/*********************************************************************
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
	//row->SetMinSize(w, 10);
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
	//row->SetMinSize(w, 10);
	row->Add(loopDescription, 1, wxEXPAND);

	wxBoxSizer* col=new wxBoxSizer(wxVERTICAL);
	col->Add(row, 1, wxALL, FromDIP(10));

	SetSizerAndFit(col);

	m_height=this->GetMinHeight();
	int w=parent->GetSize().GetWidth()-10;
	this->SetSize(w, m_height);
}

//====================================================================

BEGIN_EVENT_TABLE(CommandPanel, BasePanel)
	EVT_CHECKBOX(EvtID::ID, CommandPanel::OnCheck)
	EVT_LEFT_DOWN(CommandPanel::MouseLeftBtnDown)
	EVT_CONTEXT_MENU(CommandPanel::OnContextMenu)
END_EVENT_TABLE()

//--------------------------------------------------------------------

CommandPanel::~CommandPanel()
{
	delete m_baseCommandPtr;
	m_baseCommandPtr=nullptr;
}

//--------------------------------------------------------------------

void CommandPanel::init(bool indentation)
{
	m_isIndented=indentation;

	m_enableCmdCheck=new wxCheckBox(m_handlerPtr, EvtID::ID, wxT(" "),
					wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	m_enableCmdCheck->SetValue(m_baseCommandPtr->isActive());

	wxString description=m_baseCommandPtr->getDescription();
	m_description = new WX_TextCtrl(m_handlerPtr, wxID_ANY, description, wxDefaultPosition,
	                      //wxDefaultSize,//
	                      wxSize(-1, 23),
	                      wxNO_BORDER);

	m_description->setCallback([this](const char* val){		
		m_baseCommandPtr->updateDescription(val);
	});

	m_timeoutInput = new WX_TextCtrl(m_handlerPtr, wxID_ANY, wxT("1"), wxDefaultPosition,
								wxSize(80, 23), wxNO_BORDER, s_integerValidator);

	m_timeoutInput->setCallback([this](const char* val){
		m_baseCommandPtr->updateTime(std::atoi(val));
	});

	m_timeoutInput->Bind(wxEVT_TEXT, [this](wxCommandEvent& event) {
		wxCommandEvent event2(wxEVT_CUSTOM_EVENT, EvtID::CHANGES_MADE);
		wxPostEvent(this, event2);
    });

	m_statusBtn=new wxButton(m_handlerPtr, wxID_ANY, wxT(""),
		wxDefaultPosition, wxSize(20,20), wxNO_BORDER | wxBU_EXACTFIT);// | wxBU_NOTEXT);

	auto cmdPtr=m_baseCommandPtr;
	m_statusBtn->Bind(wxEVT_BUTTON, [cmdPtr](wxCommandEvent& evnt){
		wxString msg=wxString::Format(wxT("Error: %s."),
		ExitCode::getExitCodeMsg(cmdPtr->getExitCode()));
		wxMessageBox(msg);
	});
	m_statusBtn->Disable();

}

//--------------------------------------------------------------------

void CommandPanel::enableCommand(bool enable)
{
	m_baseCommandPtr->updateActive(enable);
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
	if(m_baseCommandPtr->getExitCode()>0){
		m_statusBtn->Enable();
		//m_statusBtn->SetLabel(wxT("🗙"));
		m_statusBtn->SetBackgroundColour(wxColour("#FF0000"));
	}
	else{
		//m_statusBtn->SetLabel(wxT("✓"));
		m_statusBtn->SetBackgroundColour(wxColour("#49d470"));
	}
}

//--------------------------------------------------------------------

void CommandPanel::OnCheck(wxCommandEvent& event)
{	
	enableCommand(m_enableCmdCheck->GetValue());
}

void CommandPanel::doIndentation(bool indentation)
{
	if(m_isIndented!=indentation){
		
		m_isIndented=indentation;

		int parentWidth=GetParent()->GetSize().GetWidth();

		m_mainCol->SetMinSize(parentWidth-2*_MARGIN_WIDTH-(c_padding*indentation), 10);

		auto paddingColNew=new wxBoxSizer(wxHORIZONTAL);
		paddingColNew->Add(c_padding*indentation, 0, wxALL, 20);

		m_sizerBody->Replace(m_paddingCol, paddingColNew);
		m_paddingCol=paddingColNew;
		m_sizerBody->Layout();
	}
}

//====================================================================

InputCommandWrapper::InputCommandWrapper(wxWindow* parent, uint posY, uint width, BaseCommand* cmd)
:CommandPanel(parent, posY, width, cmd)
{}


void InputCommandWrapper::init(bool indentation)
{
	CommandPanel::init(indentation);

	wxStaticText* paddingTime=new wxStaticText(m_handlerPtr, wxID_ANY, wxT("Wait for (ms): "));
	m_timeoutInput->ChangeValue(wxString::Format(wxT("%i"), m_baseCommandPtr->wait()));

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
	sizerVert->Add(m_handlerPtr, 1, wxEXPAND | wxTOP, _TOP_MARGIN_PADDING);
	auto sizerHor = new wxBoxSizer(wxHORIZONTAL);
	sizerHor->Add(sizerVert, 1, wxEXPAND | wxLEFT|wxRIGHT | wxBOTTOM, _MARGIN_WIDTH);
	
	this->SetSizerAndFit(sizerHor);
	m_height=this->GetMinHeight();

	int parentWidth=GetParent()->GetSize().GetWidth();

	this->SetSize(parentWidth-10, m_height);
}

//====================================================================

ControlCommandWrapper::ControlCommandWrapper(wxWindow* parent, uint posY, uint width, BaseCommand* cmd)
:CommandPanel(parent, posY, width, cmd)
{}

BEGIN_EVENT_TABLE(ControlCommandWrapper, CommandPanel)
	EVT_CHECKBOX(EvtID::ID, ControlCommandWrapper::OnCheck)
END_EVENT_TABLE()

//--------------------------------------------------------------------

void ControlCommandWrapper::init(bool indentation)
{
	CommandPanel::init(indentation);	

	wxStaticText* timeoutText=new wxStaticText(m_handlerPtr, wxID_ANY, wxT("Timeout (secs): "));

	CtrlCommand* ctrlCmdPtr=dynamic_cast<CtrlCommand*>(m_baseCommandPtr);

	updateTimeout(ctrlCmdPtr->getTimeout());

	m_edit=new wxButton(m_handlerPtr, EvtID::ID, wxT("Edit"), wxDefaultPosition,
	wxSize(30,23), wxNO_BORDER| wxBU_EXACTFIT);

	Bind(wxEVT_BUTTON, [this](wxCommandEvent& evnt){
		wxCommandEvent event(wxEVT_CUSTOM_EVENT, EvtID::EDIT_CTRL_CMD);
		event.SetClientData(m_baseCommandPtr);
		wxPostEvent(this, event);
		
	}, EvtID::ID);

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
	row2->Add(m_edit, 0, wxRIGHT, 5);
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
	sizerVert->Add(m_handlerPtr, 1, wxEXPAND | wxTOP, _TOP_MARGIN_PADDING);
	auto sizerHor = new wxBoxSizer(wxHORIZONTAL);
	sizerHor->Add(sizerVert, 1, wxEXPAND | wxLEFT |wxRIGHT | wxBOTTOM, _MARGIN_WIDTH);
	
	this->SetSizerAndFit(sizerHor);
	m_height=this->GetMinHeight();
	int parentWidth=GetParent()->GetSize().GetWidth();
	this->SetSize(parentWidth-10, m_height);
}

//--------------------------------------------------------------------

void ControlCommandWrapper::enableCommand(bool enable)
{
	CommandPanel::enableCommand(enable);
	if(enable){
		m_edit->Enable();
		return;
	}
	m_edit->Disable();
}

//====================================================================
