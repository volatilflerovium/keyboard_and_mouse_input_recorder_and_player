/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class RecorderPlayerKM                                             *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "km_recorder_player.h"

#include "event_definitions.h"
#include "wx_textctrl.h"
#include "wxstring_array.h"
#include "utilities.h"
#include "hid_manager.h"
#include "debug_utils.h"

#include <wx/display.h>
#include <wx/menu.h>
#include <wx/valnum.h>
#include <wx/spinctrl.h>

#include <filesystem>

#define FULL_SCREEN "root"
#define SCREEN_BACKGROUND "background.png"

#define CMD_LIST_WIDTH 380
#define CMD_LIST_HEIGHT 450

extern MouseEmulatorI* s_MouseEmulator;
extern KeyboardEmulatorI* s_KeyboardEmulator;

extern wxColour s_colour;
extern wxBrush s_brush;

wxTextValidator s_fileValidator(wxFILTER_EXCLUDE_CHAR_LIST);

wxTextValidator s_shortcutValidator(wxFILTER_EXCLUDE_CHAR_LIST);

extern wxIntegerValidator<unsigned int> s_integerValidator;

//====================================================================
//====================================================================

static wxBitmapBundle mkBitmapBundle(const char* icon)
{
	wxVector<wxBitmap> bitmaps;
	auto iconDirMk=[&bitmaps](const char* relPath, const char* iconName){
		std::string iconDir=resourcePath("icons/");
		iconDir.append(relPath);
		iconDir.append(iconName);
		bitmaps.push_back(wxBitmap(iconDir, wxBITMAP_TYPE_PNG));
	};
	iconDirMk("16x16/", icon);
	iconDirMk("24x24/", icon);
	iconDirMk("32x32/", icon);
	iconDirMk("48x48/", icon);
	iconDirMk("64x64/", icon);

	return wxBitmapBundle::FromBitmaps(bitmaps);
}

static wxBitmapButton* makeButton(wxWindow* parent, const char* icon, int id=-1)
{
	return new wxBitmapButton(parent, id, mkBitmapBundle(icon), wxDefaultPosition, wxSize(-1, -1));
}

//====================================================================

RecorderPlayerKM::RecorderPlayerKM(const wxString& title)
: wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition,
		wxDefaultSize,
		(wxDEFAULT_FRAME_STYLE & wxFRAME_NO_WINDOW_MENU) | wxCLOSE_BOX
	)
, m_settings(SettingsManager::getSettingManager())
, m_timer(this, WX::TIMER)
, m_playBitmapBundle(mkBitmapBundle("actions/media-playback-start-symbolic.symbolic.png"))
, m_pauseBitmapBundle(mkBitmapBundle("actions/media-playback-pause-symbolic.symbolic.png"))
, m_statusBar(nullptr)
, m_inputBlocker(nullptr)
, m_commandInputMode(CommandInputMode::REPEAT_LAST)
, m_state(State::INITIAL)
, m_currentPanelState(PanelStates::Initial)
, m_dataChanged(0)
, m_playStatus(PlayStatus::STOPPED)
, m_fullFunctionality(SystemStatus::OK)
, m_fullMenu(true)
, m_indentation(false)
{
	wxBoxSizer* mainContentSizerV = new wxBoxSizer(wxVERTICAL);

	session();

	std::error_code ec;
	if(!std::filesystem::exists(getFilePath(), ec)){
		std::filesystem::create_directories(getFilePath(), ec);
	}

	if(!std::filesystem::exists(getImgPath(), ec)){
		std::filesystem::create_directories(getImgPath(), ec);
	}

	m_settings.loadSettings();

	s_fileValidator.AddCharExcludes("/");
	s_fileValidator.AddCharExcludes("\\");
	s_fileValidator.AddCharExcludes(" ");
	s_fileValidator.SuppressBellOnError(false);

	HIDManager::SetHidEmulator(m_settings.getInterface(),
			m_settings.alpha().mb_str(), m_settings.numeric(), m_settings.isSerial());

	s_shortcutValidator.AddCharExcludes('-');
	s_shortcutValidator.AddCharExcludes('\t');

	//===============================================
	//===============================================
	//------------- check dependecies ---------------

	exeCommand<124>("import 2>&1", [this](const std::string& output){
		if(std::string::npos!=output.find("not found")){
			wxMessageBox("import: command not found.\nPlease intall ImageMagick.");
			m_fullFunctionality=SystemStatus::PARTIAL;
		}
	});

	exeCommand<124>("xwininfo -version 2>&1", [this](const std::string& output){
		if(std::string::npos!=output.find("not found")){
			wxMessageBox("xwininfo: command not found.\nPlease install x11-utils.");
			m_fullFunctionality=SystemStatus::PARTIAL;
		}
	});

	exeCommand<124>("xprop -version 2>&1", [this](const std::string& output){
		if(std::string::npos!=output.find("not found")){
			wxMessageBox("xprop: command not found.\nPlease install x11-utils.");
			m_fullFunctionality=SystemStatus::PARTIAL;
		}
	});

	//===============================================
	//--------------- recorderScreen ----------------

	m_inputBlocker=new InputBloker(this, m_settings.getTransparency());
	m_inputBlocker->setContextMenuEventHandler([this](wxContextMenuEvent& event, wxPoint mousePosition){
		OnContextMenu(event, mousePosition);
	});

	//===============================================
	//------------ ------------- --------------------

	if(m_fullFunctionality==SystemStatus::PARTIAL){
		m_commandInputMode=CommandInputMode::QUIET;
	}

	int captureOptions=m_fullFunctionality==SystemStatus::OK? 4 : 2;
	
	ArrayStringType cmdInputModeChoices(captureOptions,"");

	cmdInputModeChoices[CommandInputMode::AFTER_ME]="After Me";
	cmdInputModeChoices[CommandInputMode::QUIET]="Quiet";

	if(m_fullFunctionality==SystemStatus::OK){
		cmdInputModeChoices[CommandInputMode::REPEAT_ALL]="Repeat All";
		cmdInputModeChoices[CommandInputMode::REPEAT_LAST]="Repeat Last";
	}

	m_inputModeDropdown=new wxChoice(this, WX::COMMAND_INPUT_MODE, wxDefaultPosition,
											wxDefaultSize, cmdInputModeChoices);

	m_inputModeDropdown->SetSelection(m_commandInputMode);

	m_recordingBtn=new wxButton(this, WX::START_RECORDING, wxT("Start Recording"));

	if(m_fullFunctionality==SystemStatus::FATAL){
		m_recordingBtn->Disable();
	}

	m_selectAllCheck=new wxCheckBox(this, WX::SELECT_ALL, wxT("Select All"));
	m_invertCheck=new wxCheckBox(this, WX::INVERT, wxT("Invert"));

	m_openLoopBtn=new wxButton(this, WX::LOOP_BUTTON, wxT("Open Loop"));

	//layout
	{
		wxBoxSizer* row1 = new wxBoxSizer(wxHORIZONTAL);
		row1->Add(m_inputModeDropdown);
		row1->Add(1, 1, wxEXPAND);
		row1->Add(m_recordingBtn);

		mainContentSizerV->Add(row1, 0, wxEXPAND | wxBOTTOM, FromDIP(10));

		wxBoxSizer* row2 = new wxBoxSizer(wxHORIZONTAL);
		row2->Add(m_selectAllCheck, 0, wxRIGHT, FromDIP(10));
		row2->Add(m_invertCheck);

		wxBoxSizer* row3 = new wxBoxSizer(wxHORIZONTAL);
		row3->Add(row2, 0, wxALIGN_CENTER_VERTICAL);// |  wxEXPAND);
		row3->Add(10, 1, wxEXPAND);
		row3->Add(m_openLoopBtn);

		mainContentSizerV->Add(row3, 1, wxEXPAND);
	}

	//===============================================

	std::filesystem::path currentPath(getFilePath().c_str());
	ArrayStringType fileChoices;

	m_fileDropDown=new wxChoice(
		this,
		WX::SAVED_FILES,
		wxDefaultPosition,
		wxDefaultSize,
		fileChoices,
		wxCB_SORT
	);

	m_fileDropDown->Append(wxT(""));
	m_fileDropDown->SetSelection(0);

	int count=0;
	std::string cmdFileName;
	std::filesystem::directory_iterator dirIterator(currentPath, ec);
	if(ec.value()==0){
		for(auto const& dirEntry : dirIterator){
			if(dirEntry.is_regular_file()){
				cmdFileName=dirEntry.path().filename().c_str();
				if(cmdFileName[0]!='.'){
					count++;
					m_fileDropDown->Append(cmdFileName);
				}
			}
		}
	}

	m_fileManagerPopup=new FileListPopup(this, "File Manager");

	m_fileManagerBtn=makeButton(this, "places/folder-documents-symbolic.symbolic.png");

	m_fileManagerBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
		m_fileManagerPopup->Popup();
	});

	//layout
	{
		wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
		row->Add(m_fileDropDown, 1, wxRIGHT, FromDIP(10));
		row->Add(m_fileManagerBtn);
		mainContentSizerV->Add(row, 1, wxEXPAND | wxBOTTOM, FromDIP(10));
	}

	//-----------------------------------------------
	//--------------- Command List ------------------

	m_scrolledWindow=new ExtScrolledWindow(this, WX::CMD_LIST, wxDefaultPosition,
								FromDIP(wxSize(CMD_LIST_WIDTH, CMD_LIST_HEIGHT)));

	m_moveUpBtn=makeButton(this, "actions/go-up-symbolic.symbolic.png");

	m_moveUpBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
		if(m_scrolledWindow->swapUp()){
			m_dataChanged++;
		}
	});

	m_moveDnBtn=makeButton(this, "actions/go-down-symbolic.symbolic.png");

	m_moveDnBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
		if(m_scrolledWindow->swapDown()){
			m_dataChanged++;
		}
	});

	//layout
	{
		wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
		row->Add(m_scrolledWindow, 1, wxEXPAND | wxRIGHT, FromDIP(10));

		wxBoxSizer* col = new wxBoxSizer(wxVERTICAL);
		col->Add(m_moveUpBtn);
		col->Add(m_moveDnBtn); 

		row->Add(col);

		mainContentSizerV->Add(row, 0, wxEXPAND | wxBOTTOM, FromDIP(10));
	}

	//===============================================
	//---------------- Clear and Play ---------------

	wxBitmapButton* settingsBtn =makeButton(this, "categories/preferences-system-symbolic.symbolic.png", WX::SETTINGS);
	settingsBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
		m_settingsPopup->Popup();
	}, WX::SETTINGS);

	auto clearBtn=makeButton(this, "actions/edit-delete-symbolic.symbolic.png", WX::CLEAR);

	m_saveBtn=makeButton(this, "actions/document-save-symbolic.symbolic.png", WX::SAVE);
	m_saveBtn->Disable();
	m_saveBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
		m_saveToFilePopup->Popup();
	}, WX::SAVE);

	m_playBtn=makeButton(this, "actions/media-playback-start-symbolic.symbolic.png", WX::PLAY);
	m_playBtn->Disable();

	m_stopBtn=makeButton(this, "actions/media-playback-stop-symbolic.symbolic.png", WX::STOP);

	//layout
	{
		wxBoxSizer* row=new wxBoxSizer(wxHORIZONTAL);

		row->Add(settingsBtn);
		row->Add(1, 1, wxEXPAND);
		row->Add(m_saveBtn);
		row->Add(1, 1, wxEXPAND);
		row->Add(clearBtn);
		row->Add(1, 1, wxEXPAND);
		row->Add(m_playBtn);
		row->Add(1, 1, wxEXPAND);
		row->Add(m_stopBtn);

		mainContentSizerV->Add(row, 0, wxEXPAND | wxBOTTOM, FromDIP(10));
	}

	//===============================================
	//---------------- Close Clear ------------------

	m_keyboardBtn=makeButton(this, "devices/input-keyboard-symbolic.symbolic.png", WX::DISPLAY_KBOARD);

	if(m_commandInputMode==CommandInputMode::QUIET){
		m_keyboardBtn->Enable();
	}
	else{
		m_keyboardBtn->Disable();
	}
	m_demoBtn = new wxButton(this, WX::DEMO, wxT("Demo"));
	m_demoBtn->Disable();

	wxBitmapButton* closeBtn=makeButton(this, "actions/system-shutdown-symbolic.symbolic.png", wxID_EXIT);

	Connect(wxID_EXIT, wxEVT_COMMAND_BUTTON_CLICKED, wxCloseEventHandler(RecorderPlayerKM::OnClose));

	//layout
	{
		wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
		row->Add(m_keyboardBtn);
		row->Add(1, 1, wxEXPAND);
		row->Add(m_demoBtn);
		row->Add(1, 1, wxEXPAND);
		row->Add(closeBtn);

		mainContentSizerV->Add(row, 0, wxEXPAND | wxBOTTOM, FromDIP(10));
	}

	//===============================================
	//--------------- Status bar --------------------

	m_statusBar = new wxStaticText(this, wxID_ANY, wxT(""));
	{
		wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
		row->Add(m_statusBar, 1, wxEXPAND);

		mainContentSizerV->Add(row, 0, wxEXPAND);
	}

	if(m_fullFunctionality==SystemStatus::PARTIAL){
		m_statusBar->SetLabel(wxT("Partial functionality enabled."));
	}

	//===============================================
	//-------------------- POPUPS -------------------

	initPopups();
	
	//===============================================

	Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
		m_saveToFilePopup->Dismiss();
	}, WX::CANCEL_SAVE_FILE);

	m_saveFileDialog=new wxMessageDialog(
		this,
		wxT("Replacing it will overwrite its contents."),
		wxT("A file with the same name already exists. Do you want to replace it?"),
		wxOK|wxCENTRE|wxICON_WARNING|wxCANCEL
	);

	m_saveDataDialog=new wxMessageDialog(
		this,
		wxT("Do you want to save them before closing?"),
		wxT("The commands are not saved."),
		wxYES_NO|wxCENTRE|wxICON_WARNING|wxCANCEL
	);

	Bind(wxEVT_CUSTOM_EVENT, [this](wxCommandEvent& event){
		m_dataChanged++;
	}, EvtID::CHANGES_MADE);

	Bind(wxEVT_CUSTOM_EVENT, [this](wxCommandEvent& event){
		m_scrolledWindow->updateView(static_cast<BaseCommand*>(event.GetClientData()));
		Refresh();
	}, EvtID::UPDATE_CMD_VIEW);

	Bind(wxEVT_ACTIVATE, [this](wxActivateEvent& event){
		if(m_playStatus==PlayStatus::PLAYING){
			if(event.GetActive() ){
				Pause();
				return;
			}
		}
		event.Skip();
	});

	Bind(wxEVT_CUSTOM_EVENT, [this](wxCommandEvent& event){
		m_statusBar->SetLabel(wxString::Format(wxT("Total commands: %i"), m_scrolledWindow->getCommandCount()));
	}, EvtID::CMD_COUNT_UPDATED);

	//===============================================
	//===============================================
	//------------------ Get Focus ------------------

	//std::cout<<"got to right bottom coner to do the click\n";
	wxRect rect=wxDisplay(this).GetGeometry();
	int wx=rect.GetWidth()-2;
	int hy=rect.GetHeight()-2;
	m_getFocusCmd=[this, wx, hy](){
		/*
		s_MouseEmulator->go2Position(wx, hy, [](int& pX, int& pY){
			wxPoint mousePosition=wxGetMousePosition();
			pX=mousePosition.x;
			pY=mousePosition.y;
		});

		s_MouseEmulator->clickLeftBtn();

		s_MouseEmulator->go2Position(m_click.x, m_click.y, [](int& pX, int& pY){
			wxPoint mousePosition=wxGetMousePosition();
			pX=mousePosition.x;
			pY=mousePosition.y;
		});

		s_MouseEmulator->clickLeftBtn();// */
	};

	//===============================================
	//-----------------------------------------------

	wxBoxSizer* bx=new wxBoxSizer(wxHORIZONTAL);
	bx->Add(mainContentSizerV, 1, wxEXPAND | wxALL, FromDIP(10));
	this->SetSizerAndFit(bx);

	SetCurrentWindow(FULL_SCREEN);

	//===============================================

	ManagePanels(PanelStates::Initial);

	Centre();

	checkConnection();
	if(!m_settings.isValidInterface()){
		m_interfacePopup->Popup();
	}
}

//====================================================================

RecorderPlayerKM::~RecorderPlayerKM()
{
	m_settings.save();
	wxDELETE(m_statusBar);
	m_scrolledWindow->clear();

	// remove unused images
	std::vector<std::pair<std::string, bool> > imgVector;

	std::error_code ec;
	std::string imageFile;
	std::filesystem::directory_iterator dirIterator(getImgPath(), ec);
	if(ec.value()==0){
		for(auto& dirEntry : dirIterator){
			if(dirEntry.is_regular_file()){
				imageFile=dirEntry.path().filename().c_str();
				if(imageFile[0]!='.'){
					imgVector.push_back({imageFile.c_str(), false});
				}
			}
		}
	}
	
	std::string pattern=".png";
	pattern.append(SEPARATOR);
	wxString fileName;

	for(unsigned int i=0; i<m_fileDropDown->GetCount(); i++){
		fileName=m_fileDropDown->GetString(i);
		std::ifstream commandFiles;
		commandFiles.open(getFilePath(fileName.mb_str()), std::ifstream::in);
		if(commandFiles.is_open()){
	
			std::string commandLine;
			std::string img;
			while(std::getline(commandFiles, commandLine)){
				if(commandLine.find(pattern)!=std::string::npos){
					CstrSplit<20> parts(commandLine.c_str(), SEPARATOR);
					const char* imageName=parts[3];
					for(auto& data : imgVector){
						if(data.first==imageName){
							data.second=true;
							break;
						}
					}
				}
			}
			commandFiles.close();
		}
	}

	for(auto& data : imgVector){
		if(!data.second){
			removeImage(data.first);
		}
	}
	wxDELETE(m_roiOptions);
   wxDELETE(m_selectWindowPopup);
   wxDELETE(m_screenshotPopup);
	wxDELETE(m_auxKeyboard);
   wxDELETE(m_settingsPopup);
}

//====================================================================

BEGIN_EVENT_TABLE(RecorderPlayerKM, wxFrame)

	EVT_CLOSE(RecorderPlayerKM::OnClose)

	EVT_CHOICE(WX::COMMAND_INPUT_MODE, RecorderPlayerKM::OnModeSelection)

	EVT_BUTTON(WX::START_RECORDING, RecorderPlayerKM::OnControlBtns)
	EVT_BUTTON(WX::PLAY, RecorderPlayerKM::OnControlBtns)
	EVT_BUTTON(WX::STOP, RecorderPlayerKM::OnControlBtns)
	EVT_BUTTON(WX::CLEAR, RecorderPlayerKM::OnControlBtns)
	EVT_BUTTON(WX::DISPLAY_KBOARD, RecorderPlayerKM::OnControlBtns)

	EVT_BUTTON(WX::DEMO, RecorderPlayerKM::OnControlBtns)

	EVT_BUTTON(WX::SAVE_TO_FILE, RecorderPlayerKM::OnSave)

	EVT_TIMER(WX::TIMER, RecorderPlayerKM::OnRunCmdTimer)

	EVT_MENU(WX::MENU::WINDOW_INPUT, RecorderPlayerKM::OnMenuClick)
	EVT_MENU(WX::MENU::OPEN_LOOP, RecorderPlayerKM::OnLoopBtn)
	EVT_MENU(WX::MENU::CLOSE_LOOP, RecorderPlayerKM::OnLoopBtn)
	EVT_MENU(WX::MENU::MOVE_HERE, RecorderPlayerKM::OnMenuClick)
	EVT_MENU(WX::MENU::DO_LEFT_CLICK, RecorderPlayerKM::OnMenuClick)
	EVT_MENU(WX::MENU::DO_RIGHT_CLICK, RecorderPlayerKM::OnMenuClick)
	EVT_MENU(WX::MENU::SCREENSHOT_CMD, RecorderPlayerKM::OnMenuClick)
	EVT_MENU(WX::MENU::START_ROI, RecorderPlayerKM::OnMenuClick)
	EVT_MENU(WX::MENU::START_DRAGGING, RecorderPlayerKM::OnMenuClick)
	EVT_MENU(WX::MENU::DRAG_HERE, RecorderPlayerKM::OnMenuClick)
	EVT_MENU(WX::MENU::SUBMENU_REPEAT_ALL, RecorderPlayerKM::OnMenuClick)
	EVT_MENU(WX::MENU::SUBMENU_REPEAT_LAST, RecorderPlayerKM::OnMenuClick)
	EVT_MENU(WX::MENU::SUBMENU_AFTER_ME, RecorderPlayerKM::OnMenuClick)
	EVT_MENU(WX::MENU::SUBMENU_QUIET, RecorderPlayerKM::OnMenuClick)
	EVT_MENU(WX::MENU::CLOSE_MENU, RecorderPlayerKM::OnMenuClick)
	EVT_MENU(WX::MENU::CANCEL_NEW_ROI, RecorderPlayerKM::OnMenuClick)
	EVT_MENU(WX::MENU::DISPLAY_KBOARD, RecorderPlayerKM::OnMenuClick)

	EVT_BUTTON(WX::LOOP_BUTTON, RecorderPlayerKM::OnLoopBtn)

	EVT_BUTTON(WX::SUBMIT_INPUT, RecorderPlayerKM::OnTextInput)
	EVT_BUTTON(WX::SET_CHOICE, RecorderPlayerKM::OnSetRoiType)

	EVT_BUTTON(WX::KBD::Esc_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::ScrLk_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::Pause_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::F1_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::F2_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::F3_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::F4_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::F5_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::F6_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::F7_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::F8_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::F9_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::F10_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::F11_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::F12_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::PrtSc_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::Backspace_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::Enter_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::Insert_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::Home_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::PgUp_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::Delete_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::End_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::PgDn_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::CAPS_LOCK, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::ARROW_UP_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::ARROW_LEFT_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::ARROW_DOWN_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::ARROW_RIGHT_BTN, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::LEFT_CTRL, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::RIGHT_CTRL, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::LEFT_SHIFT, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::RIGHT_SHIFT, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::LEFT_ALT, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::RIGHT_ALT, RecorderPlayerKM::OnKeybordBtns)
	EVT_BUTTON(WX::KBD::SUPER, RecorderPlayerKM::OnKeybordBtns)

	EVT_CHECKBOX(WX::SELECT_ALL, RecorderPlayerKM::OnSelectInvert)
	EVT_CHECKBOX(WX::INVERT, RecorderPlayerKM::OnSelectInvert)

	EVT_CHOICE(WX::SAVED_FILES, RecorderPlayerKM::OnSelectedFile)

	EVT_COMMAND(EvtID::EDIT_CTRL_CMD, wxEVT_CUSTOM_EVENT, RecorderPlayerKM::OnEditCtrlCommand)
	EVT_COMMAND(EvtID::ADD_CTRL_CMD, wxEVT_CUSTOM_EVENT, RecorderPlayerKM::OnAddCtrlCmd)

	EVT_COMMAND(EvtID::NEW_SELECTION, wxEVT_SELECTION_EVENT, RecorderPlayerKM::OnSelection)
	EVT_COMMAND(EvtID::CHANGE_ROI_SCREENSHOT, wxEVT_CUSTOM_EVENT, RecorderPlayerKM::OnUpdateROI)
	EVT_COMMAND(EvtID::REMOVE_FILE_FROM_DROPDOWN, wxEVT_CUSTOM_EVENT, RecorderPlayerKM::OnUpdateFileDropdown)

END_EVENT_TABLE()

//====================================================================

bool RecorderPlayerKM::checkConnection(const char* errorMessage)
{
	bool enableFunctionality=HIDManager::connectionError([](const char* msg){
		wxMessageBox(msg);
	});

	if(HIDManager::currentEmulator(HID_TARGET::NONE)){
		enableFunctionality=false;
	}
	
	if(!enableFunctionality){
		if(m_playBtn->IsEnabled()){
			m_playBtn->Disable();
		}
		
		if(m_demoBtn->IsEnabled()){
			m_demoBtn->Disable();
		}

		m_fileDropDown->Disable();
		m_fileManagerBtn->Disable();
		m_recordingBtn->Disable();
	}
	else{
		if(m_fileDropDown->GetCount()>0){
			m_fileDropDown->Enable();
			m_fileManagerBtn->Enable();
			if(m_fileDropDown->GetSelection()>0){
				m_playBtn->Enable();
			}
		}
		m_recordingBtn->Enable();
	}
	return enableFunctionality;
}

//====================================================================

size_t RecorderPlayerKM::getFirstIndex()
{
	static int firstCommand=0;
	static CommandInputMode lastInputMode=NONE;
	if(lastInputMode!=m_commandInputMode){
		lastInputMode=m_commandInputMode;
		if(m_commandInputMode==CommandInputMode::REPEAT_ALL){
			firstCommand=m_scrolledWindow->size()-1;
		}
	}

	if(lastInputMode!=CommandInputMode::REPEAT_ALL){
		firstCommand=m_scrolledWindow->size()-1;
	}

	return firstCommand;
}

//====================================================================

void RecorderPlayerKM::OnSelection(wxCommandEvent& event)
{
	m_inputBlocker->getValues(m_selectionRect.m_posX, m_selectionRect.m_posY, m_selectionRect.m_width, m_selectionRect.m_height);
	m_selectionRect.m_posX-=m_topLeftCorner.x;
	m_selectionRect.m_posY-=m_topLeftCorner.y;

	if(m_selectionRect.isNoTrivial()){
		if(m_inputBlocker->isRectangle()){
			if(m_selectionRect.isValidRect()){
				if(m_editCtrlCmdPopup->isEditing()){
					takeRoiScreenshoot(PanelStates::Initial, 2);
				}
				else{
					m_roiOptions->Popup();
				}
			}
		}
		else{
			int endX=m_selectionRect.m_posX+m_selectionRect.m_width;
			int endY=m_selectionRect.m_posY+m_selectionRect.m_height;

			addCommand(MouseDragCommand::Builder(
				"Mouse grab/drop",
				m_settings.getTimePadding(),
				m_selectionRect.m_posX,
				m_selectionRect.m_posY,
				endX, endY,
				m_currentWindow));
		}
	}
	
	m_inputBlocker->clearSelection();
}

//====================================================================

std::string RecorderPlayerKM::imageId()
{
	static int imgCount=0;
	std::string id=session();
	id.append("_");
	id.append(std::to_string(imgCount++));
	id.append(".png");
	return id;
}

//====================================================================

void RecorderPlayerKM::OnAddCtrlCmd(wxCommandEvent& event)
{
	CtrlCommand* commandPtr=static_cast<CtrlCommand*>(event.GetClientData());
	if(commandPtr){
		addCommand<CtrlCommand>(commandPtr);
		ManagePanels(PanelStates::Recording);
	}
}

//====================================================================

void RecorderPlayerKM::addCommand()
{
	m_dataChanged++;
	m_demoBtn->Enable();
	m_playBtn->Enable();
	m_saveBtn->Enable();

	getFirstIndex();
	if(m_commandInputMode!=CommandInputMode::QUIET){
		m_playStatus=PlayStatus::PLAYING;
		RunCommands(ExtScrolledWindow::PlayMode::DEMO);
	}
	m_statusBar->SetLabel(wxString::Format(wxT("Total commands: %i"), m_scrolledWindow->getCommandCount()));
}

//====================================================================

void RecorderPlayerKM::takeRoiScreenshoot(PanelStates exitState, int roiMode)
{
	ManagePanels(PanelStates::Playing);
	m_baseImage=imageId();
	m_roiStr=m_selectionRect.getRoiStr();

	bool ok=wxTakeScreenshot(300, m_currentWindow.c_str(), m_baseImage.c_str(), m_roiStr.mb_str());

	ManagePanels(exitState);
	if(ok){
		if(roiMode==1){
			m_setupCtrlCmdPopup->loadRoi(m_baseImage.c_str(), m_roiStr.c_str(), m_currentWindow.c_str());			
			m_setupCtrlCmdPopup->Popup();
		}
		else if(roiMode==2){
			m_editCtrlCmdPopup->loadNewRoi(m_baseImage.c_str(), m_roiStr.c_str());
			m_editCtrlCmdPopup->Popup();
		}
	}
}

//====================================================================

void RecorderPlayerKM::OnLoopBtn(wxCommandEvent& event)
{
	if(WX::CANCEL_LOOP==event.GetId()){
		m_indentation=false;
		m_openLoopBtn->SetLabel(wxT("Open Loop"));
	}
	else if(m_indentation){
		m_scrolledWindow->closeLoop();
		m_openLoopBtn->SetLabel(wxT("Open Loop"));
		m_indentation=false;
	}
	else{
		m_openLoopBtn->SetLabel(wxT("Close Loop"));
		if(!m_indentation){
			m_indentation=true;
			m_scrolledWindow->addLoop(2);
		}
	}
}

//====================================================================

void RecorderPlayerKM::OnEditCtrlCommand(wxCommandEvent& event)
{
	if(!m_editCtrlCmdPopup->loadCommand(static_cast<CtrlCommand*>(event.GetClientData()))){
		return;
	}

	m_editCtrlCmdPopup->Popup();
}

//====================================================================

bool RecorderPlayerKM::SaveChangesData()
{
	if(m_dataChanged>0 ||
		(m_fileDropDown->GetCurrentSelection()==0 &&
			m_scrolledWindow->getCommandCount()>0)){
		int response=m_saveDataDialog->ShowModal();
		if(wxID_NO!=response){
			if(wxID_YES==response){
				m_saveToFilePopup->Popup();
			}
			return true;
		}
	}
	return false;
}

//====================================================================

void RecorderPlayerKM::OnSetRoiType(wxCommandEvent& evt)
{
	m_roiOptions->Dismiss();
	Update();

	SelectionType selection=static_cast<SelectionType>(m_roiRadioBox->GetSelection());

	if(selection==SELECT){
		if(m_selectionRect.isNoTrivial()){

			dbg(m_selectionRect.m_posX, " ", m_selectionRect.m_posY, " ",
				m_selectionRect.m_width, " ", m_selectionRect.m_height);

			addCommand(MouseSelectCommand::Builder("Mouse select",
				m_settings.getTimePadding(),
				m_selectionRect.m_posX, m_selectionRect.m_posY,
				m_selectionRect.m_width, m_selectionRect.m_height,
				m_currentWindow
				)
			);
		}
	}
	else if(selection==WATCH_ROI){
		if(m_selectionRect.isValidRect()){
			takeRoiScreenshoot(PanelStates::Recording, 1);
		}
	}
}

//====================================================================

void RecorderPlayerKM::OnMenuClick(wxCommandEvent& event)
{
	switch(event.GetId())
	{
		case WX::MENU::WINDOW_INPUT:
			m_selectWindowPopup->Popup();
			break;
		case WX::MENU::MOVE_HERE:
			addCommand(MoveMouseCommand::Builder("Move mouse", m_settings.getTimePadding(), m_click.x, m_click.y, m_currentWindow.c_str()));
			break;
		case WX::MENU::DO_LEFT_CLICK:
			addCommand(MouseLeftBtnCommand::Builder("Mouse left button click", m_settings.getTimePadding(), m_click.x, m_click.y, m_currentWindow.c_str()));
			break;
		case WX::MENU::DO_RIGHT_CLICK:
			addCommand(MouseRightBtnCommand::Builder("Mouse right button click", m_settings.getTimePadding(), m_click.x, m_click.y, m_currentWindow.c_str()));
			break;
		case WX::MENU::SCREENSHOT_CMD:
			m_screenshotPopup->Popup();
			break;
		case WX::MENU::START_ROI:
			m_inputBlocker->drawRectangle();
			break;
		case WX::MENU::START_DRAGGING:
			m_inputBlocker->drawLine();
			break;
		case WX::MENU::DRAG_HERE:
			addCommand(MouseDragCommand::Builder( "Mouse grab/drop to here", m_settings.getTimePadding(),
				m_click.x, m_click.y, m_currentWindow));
			break;
		case WX::MENU::CLOSE_MENU:
			ManagePanels(PanelStates::Initial);
			break;
		case WX::MENU::DISPLAY_KBOARD:
			m_auxKeyboard->Popup();
			break;
		case WX::MENU::CANCEL_NEW_ROI:
			{
				m_fullMenu=true;
				ManagePanels(PanelStates::Initial);
				if(m_editCtrlCmdPopup->isEditing()){
					m_editCtrlCmdPopup->Popup();
				}
			}
			break;
		default:
			{
				if(
					event.GetId()==WX::MENU::SUBMENU_REPEAT_ALL ||
					event.GetId()==WX::MENU::SUBMENU_REPEAT_LAST ||
					event.GetId()==WX::MENU::SUBMENU_AFTER_ME ||
					event.GetId()==WX::MENU::SUBMENU_QUIET
				){
					if(event.GetId()==WX::MENU::SUBMENU_REPEAT_ALL){
						m_commandInputMode=CommandInputMode::REPEAT_ALL;
					}
					else if(event.GetId()==WX::MENU::SUBMENU_REPEAT_LAST){
						m_commandInputMode=CommandInputMode::REPEAT_LAST;
					}
					else if(event.GetId()==WX::MENU::SUBMENU_AFTER_ME){
						m_commandInputMode=CommandInputMode::AFTER_ME;
					}
					else{
						m_commandInputMode=CommandInputMode::QUIET;
					}

					m_inputModeDropdown->SetSelection(m_commandInputMode);
					OnModeSelection(m_commandInputMode);
				}
			}
			break;
	};
}

//====================================================================

void RecorderPlayerKM::ManagePanels(PanelStates state)
{
	static bool a=false;

	m_currentPanelState=state;
	if(state==PanelStates::Recording){
		SetCurrentWindow();
		m_state=State::RECORDING;
		Iconize();
		m_inputBlocker->blockInput(true);
		if(a){
			m_auxKeyboard->Popup();
		}
	}
	else{
		a=m_auxKeyboard->isShown();
		m_auxKeyboard->Dismiss();

		m_inputBlocker->blockInput(false);
		
		if(state==PanelStates::Initial){
			m_state=State::INITIAL;
			Raise();
		}
		else if(state==PanelStates::Playing){
			Iconize();
		}
	}
	
	Refresh();
	Update();
}

//====================================================================

void RecorderPlayerKM::mkMenu(bool allowScreenshot, bool fullMenu)
{
	wxMenu menu;
	menu.SetTitle(m_currentWindow.c_str());
	if(allowScreenshot){
		if(fullMenu){
			menu.Append(WX::MENU::WINDOW_INPUT, wxT("Input on a window"));
				menu.Enable(WX::MENU::WINDOW_INPUT, m_fullFunctionality==SystemStatus::OK);

			if(!m_indentation){
				menu.Append(WX::MENU::OPEN_LOOP, wxT("Open Loop"));
			}
			else{
				menu.Append(WX::MENU::CLOSE_LOOP, wxT("Close Loop"));
			}

			menu.Append(WX::MENU::MOVE_HERE, wxT("Move Here"));
			menu.Append(WX::MENU::DO_LEFT_CLICK, wxT("Left Click Here"));
			menu.Append(WX::MENU::DO_RIGHT_CLICK, wxT("Right Click Here"));
			menu.Append(WX::MENU::START_ROI, wxT("Select Area"));
			menu.Append(WX::MENU::START_DRAGGING, wxT("Start Dragging"));
			menu.Append(WX::MENU::DRAG_HERE, wxT("Drag/Drop Here"));
				menu.Enable(WX::MENU::DRAG_HERE, false);
			menu.Append(WX::MENU::DISPLAY_KBOARD, wxT("Text and Keyboard Input"));
			menu.Append(WX::MENU::SCREENSHOT_CMD, wxT("Take Screenshot"));
				menu.Enable(WX::MENU::SCREENSHOT_CMD, allowScreenshot && m_fullFunctionality==SystemStatus::OK);

			{
				wxMenu* subMenu=new wxMenu;
				subMenu->AppendCheckItem(WX::MENU::SUBMENU_QUIET, wxT("Quiet"));
					subMenu->Check(WX::MENU::SUBMENU_QUIET, m_commandInputMode==CommandInputMode::QUIET);
				subMenu->AppendCheckItem(WX::MENU::SUBMENU_AFTER_ME, wxT("After Me"));
					subMenu->Check(WX::MENU::SUBMENU_AFTER_ME, m_commandInputMode==CommandInputMode::AFTER_ME);
				subMenu->AppendCheckItem(WX::MENU::SUBMENU_REPEAT_LAST, wxT("Repeat Last"));
					subMenu->Check(WX::MENU::SUBMENU_REPEAT_LAST, m_commandInputMode==CommandInputMode::REPEAT_LAST);
				subMenu->AppendCheckItem(WX::MENU::SUBMENU_REPEAT_ALL, wxT("Repeat All"));
					subMenu->Check(WX::MENU::SUBMENU_REPEAT_ALL, m_commandInputMode==CommandInputMode::REPEAT_ALL);
				menu.AppendSubMenu(subMenu, wxT("Recording Mode"));
			}
			menu.Append(WX::MENU::CLOSE_MENU, wxT("Close"));
		}
		else{
			menu.Append(WX::MENU::START_ROI, wxT("Select Rectangle"));
				menu.Enable(WX::MENU::START_ROI, m_fullFunctionality==SystemStatus::OK);
			menu.Append(WX::MENU::CANCEL_NEW_ROI, wxT("Close"));
		}
	}
	else{
		menu.Append(WX::MENU::WINDOW_INPUT, wxT("Input on a window"));
		menu.Append(WX::MENU::CLOSE_MENU, wxT("Close"));
	}

	PopupMenu(&menu);
}

//====================================================================

void RecorderPlayerKM::SetCurrentWindow(const char* windowName)
{
	static wxRect previousRect;

	m_cmdCaptureMode=CaptureMode::ABS;
	m_currentWindow=FULL_SCREEN;
	if(!cstrCompare(FULL_SCREEN, windowName)){		
		if(std::strlen(windowName)>0){		
			if(windowExists(windowName)){
				m_cmdCaptureMode=CaptureMode::REL;
				m_currentWindow=windowName;		
			}
		}
	}

	if(m_cmdCaptureMode==CaptureMode::REL)
	{
		WindowRect windowGeometry=getWindowRect(m_currentWindow.c_str(), true);
		m_currentWindoRect.x=windowGeometry.m_x;
		m_currentWindoRect.y=windowGeometry.m_y;
		m_currentWindoRect.width=windowGeometry.m_w;
		m_currentWindoRect.height=windowGeometry.m_h;
	}
	else{
		m_currentWindoRect=wxDisplay().GetGeometry();
	}

	m_topLeftCorner.x=m_currentWindoRect.x;
	m_topLeftCorner.y=m_currentWindoRect.y;

	if(previousRect!=m_currentWindoRect){
		previousRect=m_currentWindoRect;
		m_inputBlocker->setValidRect(m_currentWindoRect.x, m_currentWindoRect.y, m_currentWindoRect.width, m_currentWindoRect.height);
	}
}

//====================================================================

void RecorderPlayerKM::RunCommands(ExtScrolledWindow::PlayMode mode)
{
	ManagePanels(PanelStates::Playing);

	m_mode=mode;
	m_scrolledWindow->reset();

	int ms=m_settings.getTimeDelay();
	if(m_mode==ExtScrolledWindow::PlayMode::DEMO){
		m_scrolledWindow->advance2End(getFirstIndex());
		ms=500;
	}

	m_currentRunningCmd=nullptr;	
	m_timer.StartOnce(ms);
}

//====================================================================

void RecorderPlayerKM::OnRunCmdTimer(wxTimerEvent& event)
{
	if(m_playStatus!=PlayStatus::PLAYING){
		return;
	}

	if(m_currentRunningCmd==nullptr){
		if(m_scrolledWindow->getCommand(m_currentRunningCmd, m_mode)){
			m_currentRunningCmd->execute();
			m_timer.StartOnce(m_currentRunningCmd->wait());
		}
		else{
			SequenceFinished();
		}
	}
	else{
		if(m_currentRunningCmd->ready()){
			int cmdExitCode=m_currentRunningCmd->getExitCode();

			m_currentRunningCmd=nullptr;

			m_scrolledWindow->lastCommandFailed();
			
			if((cmdExitCode & 1)>0){
				SequenceFinished();
			}
			else{			
				m_timer.StartOnce(50);
			}
		}
		else{
			dbg("waiting");
			m_timer.StartOnce(m_currentRunningCmd->wait());
		}
	}
}

//====================================================================

void RecorderPlayerKM::SequenceFinished()
{
	m_playStatus=PlayStatus::STOPPED;
	m_playBtn->SetBitmap(m_playBitmapBundle);

	if(m_state!=State::RECORDING){
		ManagePanels(PanelStates::Initial);
	}
	else{
		// Notice that CommandInputMode::QUIET does not run commands
		if(m_commandInputMode!=CommandInputMode::AFTER_ME){
			std::string imageFile=SCREEN_BACKGROUND;

			bool ok=wxTakeScreenshot(m_settings.getScreenshotTimeout(), FULL_SCREEN, imageFile.c_str());
			if(ok){
				m_inputBlocker->loadBackground(getImgPath(imageFile).c_str());
			}
			else{
				m_inputBlocker->reset();
			}
		}

		ManagePanels(PanelStates::Recording);
		
		if(m_commandInputMode!=CommandInputMode::AFTER_ME){
			m_getFocusCmd();
		}
	}
}

//====================================================================

void RecorderPlayerKM::takeScreenshotByWindow(const char* windowName)
{
	if(m_fullFunctionality==SystemStatus::PARTIAL){
		return;
	}

	if(cstrCompare(windowName, FULL_SCREEN) || cstrCompare(windowName, "Full screen")){
		wxMessageBox("Control command on Full Screen not available.");
		return;
	}

	m_baseImage=imageId();
	const char* outputImage=m_baseImage.c_str();

	if(!windowExists(windowName)){
		wxMsgBox("Window with name \"%s\" not exists.", windowName);
		return;
	}

	ManagePanels(PanelStates::Playing);

	bool taken=takeScreenshot(windowName, outputImage);

	ManagePanels(PanelStates::Recording);

	if(taken){
		m_setupCtrlCmdPopup->loadRoi(m_baseImage.c_str(), "", windowName);
		m_setupCtrlCmdPopup->Popup();
	}
	else{
		removeImage(outputImage);
		wxMsgBox("Window with name \"%s\" not exists.", windowName);
	}
}



//====================================================================

void RecorderPlayerKM::windowLevelInput(const char* windowName)
{
	if(m_fullFunctionality==SystemStatus::PARTIAL){
		wxMessageBox("Some dependencies are missing");
		return;
	}

	if(cstrCompare(windowName, FULL_SCREEN) || cstrCompare(windowName, "Full screen")){
		SetCurrentWindow(FULL_SCREEN);
		m_fullMenu=true;
		SetCurrentWindow();
		ManagePanels(PanelStates::Recording);
		return;
	}

	if(!windowExists(windowName)){
		wxMsgBox("Window with name \"%s\" not exists.", windowName);
		return;
	}

	ManagePanels(PanelStates::Playing);
	const char* outputImage="windowScreenshot.png";
	bool taken=takeScreenshot(windowName, outputImage, true);

	ManagePanels(PanelStates::Recording);

	if(taken){
		m_currentWindow=windowName;
		m_windowPreviewPopup->loadImage(outputImage);
		m_windowPreviewPopup->Popup();
	}
	else{
		removeImage(outputImage);
		wxMsgBox("Window with name \"%s\" not exists.", windowName);
	}
}

//====================================================================

void RecorderPlayerKM::OnTextInput(wxCommandEvent& event)
{
	/* User should set a position command if he wants to inout text in a
	 * specific position
	 * */
	//	GetNumberOfLines
	if(m_textCmdInput->GetLineLength(0)>0){
		std::string input=std::string(m_textCmdInput->GetValue().mb_str());
		std::string partial;
		if(input.length()>16){
			partial=input.substr(0, 16);
			partial.append("...");
		}
		else{
			partial=input;
		}

		addCommand(TextCommand::Builder(partial.c_str(), m_settings.getTimePadding(), input.c_str()));
		m_textCmdInput->SetValue("");
		/* do not dismiss the keyboard popup as user could set a key input
		 * following the text input
		 * m_auxKeyboard->Dismiss();
		 */
	}
}

//====================================================================

void RecorderPlayerKM::OnKeybordBtns(wxCommandEvent& event)
{
	if(0==m_specialKeysRadioBox->GetSelection()){
		wxMessageBox(wxT("Select a valid option."));
		return;
	}

	bool isShortcut=(2==m_specialKeysRadioBox->GetSelection());

	auto processInput=[isShortcut, this](const char* keyStr, SPKEYS keyCode){
		if(isShortcut){
			wxString str=m_shortcutInput->GetValue();
			if(str.Len()>0){
				str+="+";
			}
			str+=keyStr;
			m_shortcutInput->ChangeValue(str);
		}
		else{
			addCommand(KeyCommad::Builder(keyStr, uint(keyCode), 20));
		}
	};

	switch(event.GetId())
	{
		case WX::KBD::Esc_BTN:
			addCommand(KeyCommad::Builder("Esc", uint(SPKEYS::ESC), 20));
		break;
		case WX::KBD::ScrLk_BTN:
			processInput("ScrLk", SPKEYS::SCROLLLOCK);
		break;
		case WX::KBD::Pause_BTN:
			processInput("Pause", SPKEYS::PAUSE);
		break;
		case WX::KBD::F1_BTN:
			processInput("F1", SPKEYS::F1);
		break;
		case WX::KBD::F2_BTN:
			processInput("F2", SPKEYS::F2);
		break;
		case WX::KBD::F3_BTN:
			processInput("F3", SPKEYS::F3);
		break;
		case WX::KBD::F4_BTN:
			processInput("F4", SPKEYS::F4);
		break;
		case WX::KBD::F5_BTN:
			processInput("F5", SPKEYS::F5);
		break;
		case WX::KBD::F6_BTN:
			processInput("F6", SPKEYS::F6);
		break;
		case WX::KBD::F7_BTN:
			processInput("F7", SPKEYS::F7);
		break;
		case WX::KBD::F8_BTN:
			processInput("F8", SPKEYS::F8);
		break;
		case WX::KBD::F9_BTN:
			processInput("F9", SPKEYS::F9);
		break;
		case WX::KBD::F10_BTN:
			processInput("F10", SPKEYS::F10);
		break;
		case WX::KBD::F11_BTN:
			processInput("F11", SPKEYS::F11);
		break;
		case WX::KBD::F12_BTN:
			processInput("F12", SPKEYS::F12);
		break;
		case WX::KBD::PrtSc_BTN:
			processInput("PrtSc", SPKEYS::SYSRQ);
		break;
		case WX::KBD::Backspace_BTN:
			processInput("Backspace", SPKEYS::BACKSPACE);
		break;
		case WX::KBD::Enter_BTN:// addCommand
			addCommand(KeyCommad::Builder("Enter", uint(SPKEYS::ENTER), 20));
		break;
		case WX::KBD::Insert_BTN:
			processInput("Ins", SPKEYS::INSERT);
		break;
		case WX::KBD::Home_BTN:
			processInput("Home", SPKEYS::HOME);
		break;
		case WX::KBD::PgUp_BTN:
			processInput("PgUp", SPKEYS::PAGEUP);
		break;
		case WX::KBD::Delete_BTN:
			processInput("Del", SPKEYS::DELETE);
		break;
		case WX::KBD::End_BTN:
			processInput("End", SPKEYS::END);
		break;
		case WX::KBD::PgDn_BTN:
			processInput("PgDn", SPKEYS::PAGEDOWN);
		break;
		case WX::KBD::ARROW_UP_BTN:
			processInput("UpArrow", SPKEYS::UP);
		break;
		case WX::KBD::ARROW_LEFT_BTN:
			processInput("BackArrow", SPKEYS::LEFT);
		break;
		case WX::KBD::ARROW_DOWN_BTN:
			processInput("DownArrow", SPKEYS::DOWN);
		break;
		case WX::KBD::ARROW_RIGHT_BTN:
			processInput("forwardArrow", SPKEYS::RIGHT);
		break;
		case WX::KBD::CAPS_LOCK:// addCommand
			addCommand(KeyCommad::Builder("Caps Lock", uint(SPKEYS::CAPSLOCK), 20));
		break;
		case WX::KBD::LEFT_CTRL:
			processInput("Ctrl", SPKEYS::LEFTCTRL);
		break;
		case WX::KBD::RIGHT_CTRL:
			processInput("R-Ctrl", SPKEYS::RIGHTCTRL);
		break;
		case WX::KBD::LEFT_ALT:
			processInput("Alt", SPKEYS::LEFTALT);
		break;
		case WX::KBD::RIGHT_ALT:
			processInput("R-Alt", SPKEYS::RIGHTALT);
		break;
		case WX::KBD::LEFT_SHIFT:
			processInput("Shift", SPKEYS::LEFTSHIFT);
		break;
		case WX::KBD::RIGHT_SHIFT:
			processInput("R-Shift", SPKEYS::RIGHTSHIFT);
		break;
		case WX::KBD::SUPER:
			processInput("Super", SPKEYS::LEFTMETA);
		break;
		default:
		break;
	};
}

//====================================================================

bool RecorderPlayerKM::Pause()
{
	if(m_playStatus==PlayStatus::PLAYING){
		m_playStatus=PlayStatus::PAUSED;
		m_playBtn->SetBitmap(m_playBitmapBundle);
		m_timer.Stop();
		return true;
	}
	return false;
}

//====================================================================

void RecorderPlayerKM::OnControlBtns(wxCommandEvent& event)
{
	switch(event.GetId())
	{
		case WX::PLAY:
			{
				if(m_playStatus==PlayStatus::STOPPED){
					m_playStatus=PlayStatus::PLAYING;
					m_state=State::PLAY;
					m_inputBlocker->reset();
					RunCommands(ExtScrolledWindow::PlayMode::NORMAL);					
					m_playBtn->SetBitmap(m_pauseBitmapBundle);
				}
				else{
					if(!Pause()){
						ManagePanels(PanelStates::Playing);
						m_playStatus=PlayStatus::PLAYING;
						m_playBtn->SetBitmap(m_pauseBitmapBundle);
						m_timer.StartOnce(50);
					}
				}
			}
			break;
		case WX::STOP:
			{
				m_playStatus=PlayStatus::STOPPED;
				m_playBtn->SetBitmap(m_playBitmapBundle);
			}
			break;
		case WX::DISPLAY_KBOARD:
			m_auxKeyboard->Popup();
			break;
		case WX::CLEAR:
			{
				if(SaveChangesData()){
					return;
				}
				clearCommands();
				m_openLoopBtn->SetLabel(wxT("Open Loop"));
				m_indentation=false;
				m_fileDropDown->SetSelection(0);
				m_fileNameInput->SetValue("");
				m_fileManagerPopup->setOpenFile("");
			}
			break;
		case WX::START_RECORDING:
			{
				ManagePanels(PanelStates::Recording);
				if(m_recordingBtn->GetLabel()=="Continue"){
					if(m_commandInputMode==CommandInputMode::REPEAT_ALL ||
						m_commandInputMode==CommandInputMode::REPEAT_LAST)
					{
						if(m_scrolledWindow->getCommandCount()>0){
							m_inputBlocker->clearBackground();
							m_playStatus=PlayStatus::PLAYING;
							RunCommands(ExtScrolledWindow::PlayMode::DEMO);
						}
					}
				}
				m_recordingBtn->SetLabel("Continue");
			}
			break;
		case WX::DEMO:
			{
				m_state=State::PLAY_DEMO;
				m_playStatus=PlayStatus::PLAYING;
				RunCommands(ExtScrolledWindow::PlayMode::DEMO);
			}
			break;
		default:
			break;
	};
}

//====================================================================

void RecorderPlayerKM::OnSave(wxCommandEvent& event)
{
	bool fileExists=false;
	m_saveToFilePopup->Dismiss();
	std::string input=std::string(m_fileNameInput->GetValue().mb_str());

	if(input.length()==0){
		wxMessageDialog fileNameCheck(
			this,
			wxT("Please enter a valid name"),
			wxT("Filename is empty."),
			wxOK_DEFAULT|wxCENTRE|wxICON_WARNING
		);

		if(wxID_OK==fileNameCheck.ShowModal()){
			m_saveToFilePopup->Popup();
			return;
		}
	}

	std::error_code ec;
	std::string filePath=getFilePath(input.c_str());
	if(std::filesystem::exists(filePath, ec)){
		if(wxID_OK!=m_saveFileDialog->ShowModal()){
			m_saveToFilePopup->Popup();
			return;
		}
		fileExists=true;
	}

	if(m_scrolledWindow->saveData(filePath.c_str())){
		m_dataChanged=0;
		s_fileValidator.AddExclude(input.c_str());
		if(!fileExists){
			m_fileDropDown->Append(input.c_str());
		}
		m_fileDropDown->Enable();
		m_fileManagerBtn->Enable();

		int newFileIdx=m_fileDropDown->FindString(input.c_str());//bool 	caseSensitive = false)	
		m_fileDropDown->SetSelection(newFileIdx);
		m_fileManagerPopup->setOpenFile(input.c_str());
	}
	else{
		wxMessageBox(_("Save file failed."));
	}
}

//====================================================================

void RecorderPlayerKM::OnSelectedFile(wxCommandEvent& event)
{
	static int selectedIdx=0;

	if(m_scrolledWindow->size()!=0){
		if(m_dataChanged>0){
			int response=m_saveDataDialog->ShowModal();
			if(wxID_NO!=response){
				m_fileDropDown->SetSelection(selectedIdx);
				if(wxID_YES==response){
					m_saveToFilePopup->Popup();
				}
				return;
			}
		}
		clearCommands();
	}

	selectedIdx=m_fileDropDown->GetCurrentSelection();
	wxString selectedFile=m_fileDropDown->GetString(selectedIdx);

	m_fileNameInput->SetValue(selectedFile);
	m_fileManagerPopup->setOpenFile(selectedFile.mb_str());
	if(selectedFile==""){
		return;
	}

	m_dataChanged=0;
	if(m_scrolledWindow->loadDataFile(selectedFile.mb_str())){
		m_indentation=false;
		m_demoBtn->Enable();
		m_playBtn->Enable();
		m_saveBtn->Enable();
		m_statusBar->SetLabel(wxString::Format(wxT("Total commands: %i"), m_scrolledWindow->getCommandCount()));
	}
	else{
		clearCommands();
		wxMsgBox("Failed to load the file: %s", selectedFile);
	}
}

//====================================================================

void RecorderPlayerKM::initPopups()
{
	//-----------------------------------------------
	//------------- CTRL COMMAND POPUP --------------

	m_setupCtrlCmdPopup=new AddCmdPopup(this, "New Control Command");

	//-----------------------------------------------
	//--------------- Edit Ctrl Command--------------

	m_editCtrlCmdPopup=new EditCtrlCmdPopup(this, "Edit Control Command");

	m_editCtrlCmdPopup->setOnDismissCallback([this](wxWindow*){
		m_editCtrlCmdPopup->loadCommand(nullptr);
		m_fullMenu=true;
	});

	m_editCtrlCmdPopup->setOnClose([this](){
		m_editCtrlCmdPopup->loadCommand(nullptr);
		m_fullMenu=true;
	});

	m_editCtrlCmdPopup->setOnCancel([this](){
		m_editCtrlCmdPopup->loadCommand(nullptr);
		m_fullMenu=true;
	});

	//-----------------------------------------------
	//--------------- SaveFile Popoup ---------------

	m_saveToFilePopup=new ExtendedPopup(this, "Save Commands");
	{
		auto fieldCaption=m_saveToFilePopup->builder<wxStaticText>(wxID_ANY,
								wxT("Name: "));

		m_fileNameInput=m_saveToFilePopup->builder<wxTextCtrl>(wxID_ANY, wxT(""),
							wxDefaultPosition, FromDIP(wxSize(200, 30)),
							wxTE_LEFT, s_fileValidator, wxTextCtrlNameStr);

		auto cancelFileBtn=m_saveToFilePopup->builder<wxButton>(WX::CANCEL_SAVE_FILE, wxT("Cancel"));

		auto saveFileBtn=m_saveToFilePopup->builder<wxButton>(WX::SAVE_TO_FILE, wxT("Save"));

		wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
		row->Add(fieldCaption, 0, wxALIGN_CENTER_VERTICAL);
		row->Add(m_fileNameInput, 0);

		wxBoxSizer* row1 = new wxBoxSizer(wxHORIZONTAL);
		row1->Add(cancelFileBtn,0, wxRIGHT, FromDIP(10));
		row1->Add(saveFileBtn,0);

		wxBoxSizer* col = new wxBoxSizer(wxVERTICAL);
		col->Add(row, 0, wxALL, FromDIP(10));
		col->Add(row1, 0, wxALL | wxALIGN_CENTER, FromDIP(10));

		m_saveToFilePopup->setSizer(col);
	}

	//------------------------------------------------
	//----------------- Settings ---------------------
	
	m_settingsPopup=new PopupWrapper();

	m_settingsPopup->setPopupBuilder([this](){
		auto settingsPopup= new ExtendedPopup(this, "Settings");

		auto defaultDelayTxt=settingsPopup->builder<wxStaticText>(wxID_ANY,
									wxT("Default start delay: "));

		auto defaultDelay=settingsPopup->builder<WX_TextCtrl>(wxID_ANY, wxT("1000"), wxDefaultPosition,
								FromDIP(wxSize(90, 30)), wxTE_PROCESS_ENTER, s_integerValidator);

		// diference between SetValue and ChangeValue, is that ChangeValue does NOT
		// generate text change event.
		defaultDelay->ChangeValue(wxString::Format(wxT("%i"), m_settings.getTimeDelay()));
		
		defaultDelay->setCallback([this](const char* val){
			m_settings.setTimeDelay(std::atoi(val));
		});// */

		defaultDelay->Bind(wxEVT_TEXT_ENTER, [this, defaultDelay](wxCommandEvent& event) {
			m_settings.setTimeDelay(std::atoi(defaultDelay->GetValue()));
		});

		auto defaultTimeout=settingsPopup->builder<wxStaticText>(wxID_ANY,
									wxT("Default mouse input\ntime padding: "));

		auto timePaddingSetting=settingsPopup->builder<WX_TextCtrl>(wxID_ANY, wxT("600"), wxDefaultPosition,
						FromDIP(wxSize(-1, 30)), 0, s_integerValidator);

		timePaddingSetting->SetValue(wxString::Format(wxT("%i"), m_settings.getTimePadding()));
		timePaddingSetting->setCallback([this](const char* val){
			m_settings.setTimePadding(std::atoi(val));
		});

		auto transparency=settingsPopup->builder<wxStaticText>(wxID_ANY,
									wxT("Transparency (%): "));

		auto transparencyLevelSetting=settingsPopup->builder<wxSpinCtrl>(wxID_ANY, wxT(""), wxDefaultPosition,
											wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 22);

		transparencyLevelSetting->SetValue(wxString::Format(wxT("%i"),
														(m_settings.getTransparency()*100)/255));

		transparencyLevelSetting->Bind(wxEVT_SPINCTRL, [this, transparencyLevelSetting](wxSpinEvent& event){
			int transparency=transparencyLevelSetting->GetValue();
			m_settings.setTransparency((255*transparency)/100);
			m_inputBlocker->setTransparency(m_settings.getTransparency());
		});

		auto screenshotTag=settingsPopup->builder<wxStaticText>(wxID_ANY,
									wxT("Screenshot timeout (ms): "));

		auto screenshotTimeoutSetting=settingsPopup->builder<WX_TextCtrl>(wxID_ANY, wxT("500"), wxDefaultPosition,
						FromDIP(wxSize(-1, 30)), 0, s_integerValidator);

		screenshotTimeoutSetting->ChangeValue(wxString::Format(wxT("%i"), m_settings.getScreenshotTimeout()));
		screenshotTimeoutSetting->setCallback([this](const char* val){
			m_settings.setScreenshotTimeout(std::atoi(val));
		});

		auto selectionBrushColourTag=settingsPopup->builder<wxStaticText>(wxID_ANY,
									wxT("Selection colour: "));

		auto selectionBrushColour=settingsPopup->builder<WX_TextCtrl>(wxID_ANY, m_settings.getBrushColour(), wxDefaultPosition,
								wxDefaultSize, wxTE_PROCESS_ENTER);

		selectionBrushColour->SetBackgroundColour(wxColor(m_settings.getBrushColour()));

		selectionBrushColour->setCallback([this, selectionBrushColour](const char* val){
			if(isHex(val)){
				m_settings.setBrushColour(val);
				selectionBrushColour->SetBackgroundColour(wxColour(val));
				s_colour=wxColor(val);
				s_brush=wxBrush(wxColor(val), wxBRUSHSTYLE_CROSSDIAG_HATCH);
			}
		});

		auto interfacePopupBtn=settingsPopup->builder<wxButton>(wxID_ANY, wxT("Set Interface"));

		interfacePopupBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
			m_settingsPopup->Dismiss();
			m_interfacePopup->Popup();
		});

		// Layout
		{
			wxBoxSizer* row=new wxBoxSizer(wxHORIZONTAL);
			row->Add(defaultDelayTxt, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(10));
			row->Add(defaultDelay, 1);

			wxBoxSizer* row1=new wxBoxSizer(wxHORIZONTAL);
			row1->Add(defaultTimeout, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(10));
			row1->Add(timePaddingSetting, 1);
			
			wxBoxSizer* row2=new wxBoxSizer(wxHORIZONTAL);
			row2->Add(transparency, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(10));
			row2->Add(transparencyLevelSetting, 1);

			wxBoxSizer* row3=new wxBoxSizer(wxHORIZONTAL);
			row3->Add(screenshotTag, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(10));
			row3->Add(screenshotTimeoutSetting, 1);

			wxBoxSizer* row4=new wxBoxSizer(wxHORIZONTAL);
			row4->Add(selectionBrushColourTag, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(10));
			row4->Add(selectionBrushColour, 1);

			wxBoxSizer* row5=new wxBoxSizer(wxHORIZONTAL);
			row5->Add(interfacePopupBtn, 0);

			wxBoxSizer* col = new wxBoxSizer(wxVERTICAL);
			col->Add(row, 0, wxBOTTOM | wxEXPAND, FromDIP(10));
			col->Add(row1, 0, wxBOTTOM | wxEXPAND, FromDIP(10));
			col->Add(row2, 0, wxBOTTOM | wxEXPAND, FromDIP(10));
			col->Add(row3, 0, wxBOTTOM | wxEXPAND, FromDIP(10));
			col->Add(row4, 0, wxBOTTOM | wxEXPAND, FromDIP(10));
			col->Add(row5, 0);

			settingsPopup->setSizer(col);
		}

		return settingsPopup;
	});

	//------------------------------------------------
	//----------------- Interface --------------------

	m_interfacePopup= new ExtendedPopup(this, "Interface");

	auto serialPortTag=m_interfacePopup->builder<wxStaticText>(wxID_ANY, "");

	auto serialPort=m_interfacePopup->builder<wxTextCtrl>(wxID_ANY, "", wxDefaultPosition,
							wxDefaultSize);
	serialPort->ChangeValue(m_settings.alpha());
	
	auto baudRateTag=m_interfacePopup->builder<wxStaticText>(wxID_ANY, "");

	auto baudRate=m_interfacePopup->builder<wxTextCtrl>(wxID_ANY, "", wxDefaultPosition,
							wxDefaultSize, 0, s_integerValidator);
	baudRate->ChangeValue(wxString::Format(wxT("%i"),m_settings.numeric()));
	
	if(m_settings.isTinyusbLink()){
		serialPort->Enable();
		baudRate->Enable();
		if(m_settings.isUDP()){
			serialPortTag->SetLabel(wxT("ip: "));
			baudRateTag->SetLabel(wxT("port: "));		
		}
		else{
			serialPortTag->SetLabel(wxT("serial port: "));
			baudRateTag->SetLabel(wxT("baud rate: "));
		}
	}
	else{
		serialPort->Disable();
		baudRate->Disable();
	}

	auto connectionTag=m_interfacePopup->builder<wxStaticText>(wxID_ANY,
								wxT("Interface via: "));
	{
		ArrayStringType choices(4,"");
		choices[0]="none";
		choices[1]="/dev/uinput";
		choices[2]="tinyusb serial";
		choices[3]="tinyusb udp";
		auto connectionRadioBox=m_interfacePopup->builder<wxRadioBox>(wxID_ANY, "",
				wxDefaultPosition, wxDefaultSize, choices, 2, wxRA_HORIZONTAL);
		connectionRadioBox->SetSelection(m_settings.getInterface<uint>());

		connectionRadioBox->Bind(wxEVT_RADIOBOX, [this, serialPortTag, baudRateTag, baudRate, serialPort, connectionRadioBox](wxCommandEvent& event){
			if(1<connectionRadioBox->GetSelection()){
				int j=connectionRadioBox->GetSelection();
				baudRate->Enable();
				baudRate->ChangeValue(wxString::Format(wxT("%i"),m_settings.numeric(j)));
				serialPort->Enable();
				serialPort->ChangeValue(m_settings.alpha(j));
			
				if(2==connectionRadioBox->GetSelection()){
					serialPortTag->SetLabel(wxT("Serial port: "));
					baudRateTag->SetLabel(wxT("Baud rate: "));
				}
				else{
					serialPortTag->SetLabel(wxT("IP: "));
					baudRateTag->SetLabel(wxT("Port: "));
				}
			}
			else{
				baudRate->Disable();
				serialPort->Disable();
			}
		});

		auto setConnection=m_interfacePopup->builder<wxButton>(wxID_ANY, wxT("Set"));

		setConnection->Bind(wxEVT_BUTTON, [this, connectionRadioBox, baudRate, serialPort](wxCommandEvent& event){
			bool ok=true;

			m_settings.setInterfaceSetting(
					connectionRadioBox->GetSelection(), serialPort->GetValue(), wxAtoi(baudRate->GetValue()));

			if(m_settings.isTinyusbLink()){
				if(serialPort->GetValue().length()==0 || wxAtoi(baudRate->GetValue())<1){
					ok=false;
				}
			}

			if(checkConnection() && ok){
				m_interfacePopup->Dismiss();
			}
		});

		wxBoxSizer* row1=new wxBoxSizer(wxHORIZONTAL);
		row1->Add(connectionTag, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(10));
		row1->Add(connectionRadioBox, 1, wxALIGN_CENTER_VERTICAL);

		wxBoxSizer* row2=new wxBoxSizer(wxHORIZONTAL);
		row2->Add(serialPortTag, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(10));
		row2->Add(serialPort, 1);

		wxBoxSizer* row3=new wxBoxSizer(wxHORIZONTAL);
		row3->Add(baudRateTag, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(10));
		row3->Add(baudRate, 1);

		wxBoxSizer* row4=new wxBoxSizer(wxHORIZONTAL);
		row4->Add(setConnection, 0);

		wxBoxSizer* col = new wxBoxSizer(wxVERTICAL);
		col->Add(row1, 0, wxBOTTOM | wxEXPAND, FromDIP(10));
		col->Add(row2, 0, wxBOTTOM | wxEXPAND, FromDIP(10));
		col->Add(row3, 0, wxBOTTOM | wxEXPAND, FromDIP(10));
		col->Add(row4, 0, wxALIGN_RIGHT);

		m_interfacePopup->setSizer(col);
	}

	//-----------------------------------------------
	//----------- selection types Popup -------------

	m_roiOptions=new PopupWrapper();

	m_roiOptions->setPopupBuilder([this](){
		auto roiOptions=new ExtendedPopup(this, "Selection Types");
		{
			ArrayStringType choices(2,"");
			choices[SELECT]="Select";
			choices[WATCH_ROI]="Watch ROI";

			m_roiRadioBox = roiOptions->builder<wxRadioBox>(WX::ROI_RADIO, "",
							wxDefaultPosition, wxDefaultSize, choices, 2, wxRA_HORIZONTAL);

			m_roiRadioBox->SetSelection(SELECT);
			if(m_fullFunctionality==SystemStatus::PARTIAL){
				m_roiRadioBox->Disable();
			}

			wxBoxSizer* radioRow=new wxBoxSizer(wxHORIZONTAL);
			radioRow->Add(m_roiRadioBox, 0);
			auto setBtn =roiOptions->builder<wxButton>(WX::SET_CHOICE, wxT("Set"));

			wxBoxSizer* col = new wxBoxSizer(wxVERTICAL);

			col->Add(radioRow, 0, wxALL, FromDIP(10));
			col->Add(setBtn, 0, wxBOTTOM | wxALIGN_CENTER, FromDIP(10));

			roiOptions->setSizer(col);
		}
		return roiOptions;
	});

	//-----------------------------------------------
	//----------------- Screenshoot Popup --------------

	m_screenshotPopup=new PopupWrapper();

	m_screenshotPopup->setPopupBuilder([this](){
		auto screenshotPopup=new ExtendedPopup(this, "Screenshot");
		{
			auto fieldCaption=screenshotPopup->builder<wxStaticText>(wxID_ANY,
									wxT("Window name: "));

			m_screenshotInput=screenshotPopup->builder<wxTextCtrl>(wxID_ANY, wxT(""),
								wxDefaultPosition, FromDIP(wxSize(250, 30)),
								wxTE_LEFT, s_fileValidator);

			auto cancelBtn =screenshotPopup->builder<wxButton>(wxID_ANY, wxT("Cancel"));
			cancelBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
				m_screenshotPopup->Dismiss();
				m_screenshotInput->ChangeValue("");
			});

			auto takeScreenshotBtn=screenshotPopup->builder<wxButton>(wxID_ANY, wxT("OK"));
			takeScreenshotBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
				m_screenshotPopup->Dismiss();
				Update();
				if(m_screenshotInput->GetValue().length()>0){
					takeScreenshotByWindow(m_screenshotInput->GetValue().mb_str());			
				}
			});

			wxBoxSizer* col0 = new wxBoxSizer(wxVERTICAL);
			col0->Add(fieldCaption, 0, wxBOTTOM, FromDIP(10));
			col0->Add(m_screenshotInput, 0);

			wxBoxSizer* row1 = new wxBoxSizer(wxHORIZONTAL);
			row1->Add(cancelBtn,0, wxRIGHT, FromDIP(10));
			row1->Add(takeScreenshotBtn,0);

			wxBoxSizer* col = new wxBoxSizer(wxVERTICAL);
			col->Add(col0, 0, wxALL, FromDIP(10));
			col->Add(row1, 0, wxALL | wxALIGN_CENTER, FromDIP(10));

			screenshotPopup->setSizer(col);
		}
		return screenshotPopup;
	});

	//-----------------------------------------------
	//---------------Select Window-------------------

	m_selectWindowPopup=new PopupWrapper();

	m_selectWindowPopup->setPopupBuilder([this](){

		auto selectWindowPopup=new ExtendedPopup(this, "Select Window");
		{
			auto fieldCaption=selectWindowPopup->builder<wxStaticText>(wxID_ANY,
									wxT("Window name: "));

			ArrayStringType choices2(1, "Full screen");

			m_windowNameInput=selectWindowPopup->builder<wxComboBox>(wxID_ANY, _T(""),
					wxDefaultPosition, FromDIP(wxSize(300, 30)), choices2, wxTE_PROCESS_ENTER
					);

			auto cancelBtn=selectWindowPopup->builder<wxButton>(wxID_ANY, wxT("Cancel"));
			cancelBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
				m_selectWindowPopup->Dismiss();
				m_windowNameInput->ChangeValue("");
			});

			auto takeScreenshotBtn=selectWindowPopup->builder<wxButton>(wxID_ANY, wxT("OK"));

			takeScreenshotBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
				m_selectWindowPopup->Dismiss();
				Update();
				windowLevelInput(m_windowNameInput->GetValue().mb_str());
			});

			wxBoxSizer* col0 = new wxBoxSizer(wxVERTICAL);
			col0->Add(fieldCaption, 0, wxBOTTOM, FromDIP(10));
			col0->Add(m_windowNameInput, 0);

			wxBoxSizer* row1 = new wxBoxSizer(wxHORIZONTAL);
			row1->Add(cancelBtn,0, wxRIGHT, FromDIP(10));
			row1->Add(takeScreenshotBtn,0);

			wxBoxSizer* col = new wxBoxSizer(wxVERTICAL);
			col->Add(col0, 0, wxALL, FromDIP(10));
			col->Add(row1, 0, wxALL | wxALIGN_CENTER, FromDIP(10));

			selectWindowPopup->setSizer(col);
		}

		return selectWindowPopup;
	});

	//-----------------------------------------------
	//--------------Window Preview-------------------

	m_windowPreviewPopup=new WindowPreview(this, "Window Preview");

	m_windowPreviewPopup->setOnClose([this](){
		m_fullMenu=true;
		SetCurrentWindow(FULL_SCREEN);
	});

	m_windowPreviewPopup->bindCancelBtn([this](wxCommandEvent& event){
		m_windowPreviewPopup->Dismiss();
		m_fullMenu=true;
		SetCurrentWindow(FULL_SCREEN);
	});

	m_windowPreviewPopup->bindOkBtn([this](wxCommandEvent& event){
		m_windowPreviewPopup->Dismiss();
		SetCurrentWindow();
		wxString str=m_currentWindow.c_str();
		m_selectWindowPopup->init();
		if(wxNOT_FOUND==m_windowNameInput->FindString(str)){
			m_windowNameInput->Append(str);
		}
	});

	//-----------------------------------------------
	//----------------- Keyboard Popup --------------

	m_auxKeyboard=new PopupWrapper;

	m_auxKeyboard->setPopupBuilder([this](){

		auto auxKeyboard=new ExtendedPopup(this, "Text and Key Input");
		auxKeyboard->SetFont(GetFont().Scale(0.75));
	
		wxBoxSizer* kbrdSizerV=new wxBoxSizer(wxVERTICAL);

		m_textCmdInput = auxKeyboard->builder<wxTextCtrl>(wxID_ANY, wxT(""), wxDefaultPosition,
								FromDIP(wxSize(250, 50)), wxTE_MULTILINE | wxTE_PROCESS_TAB);//, wxDefaultValidator);

		wxButton* submitBtn = auxKeyboard->builder<wxButton>(WX::SUBMIT_INPUT, wxT("OK"));

		wxTextCtrl* unicodeInput=auxKeyboard->builder<wxTextCtrl>(wxID_ANY, wxT(""),
								wxDefaultPosition, FromDIP(wxSize(70, 30)),
								wxTE_PROCESS_ENTER, wxDefaultValidator, wxTextCtrlNameStr);

		unicodeInput->SetHint(wxT("XXXX"));

		unicodeInput->Bind(wxEVT_TEXT_ENTER, [unicodeInput, this](wxCommandEvent& event) {

			if(unicodeInput->GetValue().length()>0 &&
				unicodeInput->GetValue().find(" ")==std::string::npos){
				std::string description("Unicode: ");
				description.append(unicodeInput->GetValue().Lower().mb_str());
				addCommand(UnicodeCommand::Builder(description.c_str(),
									m_settings.getTimePadding(), unicodeInput->GetValue().Lower().mb_str()));

				unicodeInput->SetValue("");
			}
		});

		m_shortcutInput=auxKeyboard->builder<wxTextCtrl>(wxID_ANY, wxT(""),
								wxDefaultPosition, FromDIP(wxSize(180, 30)),
								wxTE_PROCESS_ENTER, s_shortcutValidator);

		m_shortcutInput->SetHint(wxT("key1+...+key5"));

		m_shortcutInput->Bind(wxEVT_TEXT_ENTER, [this](wxCommandEvent& event) {
			addCommand(ShortcutCommand::Builder(
								m_shortcutInput->GetValue().Upper().mb_str(),
								m_settings.getTimePadding(),
								m_shortcutInput->GetValue().Upper().mb_str()
								)
						);
				m_shortcutInput->ChangeValue("");
		});

		wxStaticText* unicodeText=auxKeyboard->builder<wxStaticText>(wxID_ANY, wxT("Unicode:"));
		wxStaticText* shortcutText=auxKeyboard->builder<wxStaticText>(wxID_ANY, wxT("Shortcut:"));

		{
			wxBoxSizer* unicodeCol = new wxBoxSizer(wxVERTICAL);
			unicodeCol->Add(unicodeText, 0, wxBOTTOM, FromDIP(5));
			unicodeCol->Add(unicodeInput, 0);

			wxBoxSizer* shortcutCol = new wxBoxSizer(wxVERTICAL);
			shortcutCol->Add(shortcutText, 0, wxBOTTOM, FromDIP(5));
			shortcutCol->Add(m_shortcutInput, 0);

			wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
			row->Add(m_textCmdInput, 0, wxRIGHT | wxEXPAND, FromDIP(10));
			row->Add(submitBtn, 0, wxRIGHT, FromDIP(10));
			row->Add(unicodeCol, 0, wxRIGHT, FromDIP(10));
			row->Add(shortcutCol, 0, FromDIP(10));
			kbrdSizerV->Add(row, 0, FromDIP(10));
		}

		{
			ArrayStringType choices(3, "");
			choices[0]="Select";
			choices[1]="Single command";
			choices[2]="Shortcut";

			m_specialKeysRadioBox=auxKeyboard->builder<wxRadioBox>(wxID_ANY, "",
						wxDefaultPosition, wxDefaultSize, choices, 3, wxRA_HORIZONTAL);

			m_specialKeysRadioBox->SetSelection(0);

			wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
			row->Add(m_specialKeysRadioBox, 0);
			kbrdSizerV->Add(row, 0, wxCENTER, FromDIP(10));
		}

		wxBoxSizer* kbrdSizer=new wxBoxSizer(wxHORIZONTAL);

		std::pair<int, wxString> fButtons[]={
			{WX::KBD::Esc_BTN, wxT("Esc")},
			{WX::KBD::ScrLk_BTN, wxT("ScrLk")},
			{WX::KBD::Pause_BTN, wxT("Pause")},
			{WX::KBD::F1_BTN, wxT("F1")},
			{WX::KBD::F2_BTN, wxT("F2")},
			{WX::KBD::F3_BTN, wxT("F3")},
			{WX::KBD::F4_BTN, wxT("F4")},
			{WX::KBD::F5_BTN, wxT("F5")},
			{WX::KBD::F6_BTN, wxT("F6")},
			{WX::KBD::F7_BTN, wxT("F7")},
			{WX::KBD::F8_BTN, wxT("F8")},
			{WX::KBD::F9_BTN, wxT("F9")},
			{WX::KBD::F10_BTN, wxT("F10")},
			{WX::KBD::F11_BTN, wxT("F11")},
			{WX::KBD::F12_BTN, wxT("F12")}
		};
		wxBoxSizer* fButtonCol=new wxBoxSizer(wxVERTICAL);
		{
			wxBoxSizer* buttonRow;
			for(int i=0; i<5; i++){
				buttonRow=new wxBoxSizer(wxHORIZONTAL);
				for(int j=0; j<3; j++){
					wxButton* btn1 = auxKeyboard->builder<wxButton>(fButtons[i*3+j].first, fButtons[i*3+j].second, //);
					 wxDefaultPosition);//, wxDefaultSize, wxBU_EXACTFIT);
					buttonRow->Add(btn1, 0, wxALL, FromDIP(1));
				}
				fButtonCol->Add(buttonRow, 0, wxALL | wxEXPAND, FromDIP(1));
			}
		}

		kbrdSizer->Add(fButtonCol, 0, wxALL, FromDIP(2));

		//-----------------------------------------------

			wxBoxSizer* buttonCol=new wxBoxSizer(wxVERTICAL);

			auto mkRowBtns=[this, auxKeyboard](wxBoxSizer* buttonCol, std::pair<int, wxString>* btnData, int length){
				wxBoxSizer* buttonRow=new wxBoxSizer(wxHORIZONTAL);
					for(int j=0; j<length; j++){
						wxButton* btn1 =auxKeyboard->builder<wxButton>(btnData[j].first, btnData[j].second);
						buttonRow->Add(btn1, 0, wxALL, FromDIP(1));
					}
					buttonCol->Add(buttonRow, 0, wxALL | wxCENTER, FromDIP(1));
				};

			{
				std::pair<int, wxString> buttons[]={
					{WX::KBD::PrtSc_BTN, wxT("PrtSc")},
					{WX::KBD::Backspace_BTN, wxT("Backspace")},
					{WX::KBD::Enter_BTN, wxT("Enter")}
				};
				int totalBtns=sizeof(buttons)/sizeof(std::pair<int, wxString>);
				mkRowBtns(buttonCol, buttons, totalBtns);
			}

			{
				std::pair<int, wxString> buttons[]={
					{WX::KBD::LEFT_SHIFT, wxT("Shift")},
					{WX::KBD::Insert_BTN, wxT("Insert")},
					{WX::KBD::Home_BTN, wxT("Home")},
					{WX::KBD::PgUp_BTN, wxT("PgUp")},
					{WX::KBD::RIGHT_SHIFT, wxT("Shift")}
				};
				int totalBtns=sizeof(buttons)/sizeof(std::pair<int, wxString>);
				mkRowBtns(buttonCol, buttons, totalBtns);
			}

			{
				std::pair<int, wxString> buttons[]={
					{WX::KBD::LEFT_CTRL, wxT("Ctrl")},
					{WX::KBD::Delete_BTN, wxT("Delete")},
					{WX::KBD::End_BTN, wxT("End")},
					{WX::KBD::PgDn_BTN, wxT("PgDn")},
					{WX::KBD::RIGHT_CTRL, wxT("Ctrl")}
				};
				int totalBtns=sizeof(buttons)/sizeof(std::pair<int, wxString>);
				mkRowBtns(buttonCol, buttons, totalBtns);
			}

			{
				std::pair<int, wxString> buttons[]={
					{WX::KBD::LEFT_ALT, wxT("Alt")},
					{WX::KBD::CAPS_LOCK, wxT("CapsLk")},
					{WX::KBD::ARROW_UP_BTN, wxT("A Up")},
					{WX::KBD::SUPER, wxT("Super")},
					{WX::KBD::RIGHT_ALT, wxT("Alt")}
				};
				int totalBtns=sizeof(buttons)/sizeof(std::pair<int, wxString>);
				mkRowBtns(buttonCol, buttons, totalBtns);
			}

			{
				std::pair<int, wxString> buttons[]={
					{WX::KBD::ARROW_LEFT_BTN, wxT("A Lt")},
					{WX::KBD::ARROW_DOWN_BTN, wxT("A Dn")},
					{WX::KBD::ARROW_RIGHT_BTN, wxT("A Rt")}
				};
				int totalBtns=sizeof(buttons)/sizeof(std::pair<int, wxString>);
				mkRowBtns(buttonCol, buttons, totalBtns);
			}

		kbrdSizer->Add(buttonCol, 0, wxALL, FromDIP(5));

		kbrdSizerV->Add(kbrdSizer, 0);

		auxKeyboard->setSizer(kbrdSizerV);

		wxRect rect=wxDisplay(this).GetGeometry();
		wxSize sz=auxKeyboard->GetSize();
		wxPoint pt;
		pt.x=(rect.GetWidth()-sz.x)/2;
		pt.y=(rect.GetHeight() -sz.y)/2;
		m_click=pt;
		auxKeyboard->SetPosition(pt);
	
		return auxKeyboard;
	});
}

//====================================================================

void RecorderPlayerKM::clearCommands()
{
	m_statusBar->SetLabel("");

	m_dataChanged=0;
	m_inputBlocker->reset();

	m_scrolledWindow->clear();

	m_recordingBtn->SetLabel(_T("Start Recording"));
	m_demoBtn->Disable();
	m_playBtn->Disable();
	m_saveBtn->Disable();
}

//====================================================================

void RecorderPlayerKM::OnModeSelection(CommandInputMode mode)
{
	m_commandInputMode=mode;
	if(m_commandInputMode==CommandInputMode::QUIET){
		m_keyboardBtn->Enable();
		m_inputBlocker->reset();
	}
	else if(m_commandInputMode==CommandInputMode::AFTER_ME){
		m_inputBlocker->reset();
	}
	else{
		m_keyboardBtn->Disable();
	}
}

//====================================================================

const char* RecorderPlayerKM::session(bool regenerate)
{
	static std::string session=getTimeStamp("%y%m%d%H%M%S");
	if(regenerate){
		session=getTimeStamp("%y%m%d%H%M%S");
	}
	return session.c_str();
}

//====================================================================
