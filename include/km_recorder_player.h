/*********************************************************************
* class RecorderPlayerKM                                             *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _KM_RECORDER_PLAYER_GUI_H
#define _KM_RECORDER_PLAYER_GUI_H

#include "image_panel.h"
#include "inputblocker.h"
#include "input_command.h"
#include "ext_scrolled_window.h"
#include "dedicated_popups.h"
#include "enumerations.h"
#include "utilities.h"
#include "settings_manager.h"
#include "wx_utils.h"

#include <wx/wx.h>
#include <wx/colour.h>

//--------------------------------------------------------------------

class EditCtrlCmdPopup;
class AddCmdPopup;

class RecorderPlayerKM : public wxFrame
{
	public:
		RecorderPlayerKM(const wxString& title);
		virtual ~RecorderPlayerKM();

	private:
		enum SelectionType
		{
			SELECT,
			WATCH_ROI,
		};

		enum CaptureMode
		{
			ABS,
			REL,
		};

		enum class PanelStates
		{
			Initial,
			Recording,
			Playing,
		};

		enum CommandInputMode
		{
			QUIET,
			AFTER_ME, // same as repeat last but not screenshot is taken
			REPEAT_LAST,	
			REPEAT_ALL,
			NONE,// not use
		};

		enum class State
		{
			INITIAL,
			RECORDING,
			PLAY,
			PLAY_DEMO,
		};

		enum class PlayStatus
		{
			STOPPED,
			PLAYING,
			PAUSED,
		};

		struct ROI // replace with wxRect?
		{
			int m_posX;
			int m_posY;
			int m_width;
			int m_height;

			bool isValidRect()
			{
				return m_width*m_height!=0;
			}

			bool isNoTrivial()
			{
				return m_width!=0 || m_height!=0;
			}

			wxString getRoiStr()
			{
				return wxString::Format(wxT("%ix%i+%i+%i"), m_width, m_height, m_posX, m_posY);
			}
		};

		enum SystemStatus
		{
			OK,
			PARTIAL,
			FATAL,
		};

		SettingsManager& m_settings;

	private:
		wxTimer m_timer;

		std::string m_baseImage;
		wxString m_roiStr;
		wxString m_currentWindow;

		wxRect m_currentWindoRect;
		ROI m_selectionRect;
		wxPoint m_topLeftCorner;//of the current window 
		wxPoint m_click;

		wxBitmapBundle m_playBitmapBundle;
		wxBitmapBundle m_pauseBitmapBundle;

		wxButton* m_recordingBtn;
		wxButton* m_openLoopBtn;
		wxButton* m_demoBtn;
		wxBitmapButton* m_stopBtn;
		wxBitmapButton* m_playBtn;
		wxBitmapButton* m_keyboardBtn;
		wxBitmapButton* m_fileManagerBtn;

		wxButton* m_updateCtrlCmdBtn;
		wxButton* m_addScreenshotCondition;
		wxBitmapButton* m_saveBtn;
		wxBitmapButton* m_moveDnBtn;
		wxBitmapButton* m_moveUpBtn;

		wxStaticText* m_statusBar;

		wxTextCtrl* m_textCmdInput;
		wxTextCtrl* m_loopEditInput;
		wxTextCtrl* m_fileNameInput;
		wxTextCtrl* m_shortcutInput;
		wxTextCtrl* m_screenshotInput;
		wxComboBox* m_windowNameInput;

		ExtScrolledWindow* m_scrolledWindow;

		wxCheckBox* m_selectAllCheck;
		wxCheckBox* m_invertCheck;
		wxRadioBox* m_roiRadioBox;
		wxRadioBox* m_specialKeysRadioBox;

		wxChoice* m_inputModeDropdown;
		wxComboBox* m_shortcutDropDown;
      PopupWrapper* m_roiOptions;
      PopupWrapper* m_selectWindowPopup;
      PopupWrapper* m_screenshotPopup;
      
		PopupWrapper* m_auxKeyboard;
      PopupWrapper* m_settingsPopup;
      
		ExtendedPopup* m_saveToFilePopup;
		WindowPreview* m_windowPreviewPopup;
		AddCmdPopup* m_setupCtrlCmdPopup;
		EditCtrlCmdPopup* m_editCtrlCmdPopup;
		ExtendedPopup* m_interfacePopup;

		FileListPopup* m_fileManagerPopup;

		InputBloker* m_inputBlocker;

		BaseCommand* m_currentRunningCmd;

		wxChoice* m_fileDropDown;
		wxMessageDialog* m_saveFileDialog;
		wxMessageDialog* m_saveDataDialog;

		ExtScrolledWindow::PlayMode m_mode;
		Cmd m_getFocusCmd;

		CommandInputMode m_commandInputMode;
		State m_state;
		PanelStates m_currentPanelState;
		PanelStates m_exitState;

		int m_dataChanged;
		PlayStatus m_playStatus;
		CaptureMode m_cmdCaptureMode;
		SystemStatus m_fullFunctionality;

		bool m_fullMenu;
		bool m_indentation;

		bool checkConnection(const char* errorMessage="");
		void initPopups();
		bool Pause();

		template<typename FUNC>
		void windowInputScreenshot(const char* windowName, const char* outputImage, FUNC cbk);

		void OnModeSelection(CommandInputMode mode);

		void SetCurrentWindow();
		void SetCurrentWindow(const char* windowName);

		bool SaveChangesData();
		void clearCommands();

		const char* session(bool regenerate=false);
		std::string imageId();

		void windowLevelInput(const char* windowName);
		void takeScreenshotByWindow(const char* windowName);

		void takeRoiScreenshoot(PanelStates exitState, int roiMode);
		void RunCommands(ExtScrolledWindow::PlayMode mode);
		void SequenceFinished();

		void mkMenu(bool allowScreenshot, bool fullMenu);
		size_t getFirstIndex();

		void addCommand();
		template<typename T=InputCommand>
		void addCommand(BaseCommand* cmd);

		void StartWorker();
		void postEvent(wxEventType commandEventType, int id);

		void OnUpdateFileDropdown(wxCommandEvent& event);
		void OnSelection(wxCommandEvent& event);
		void OnUpdateROI(wxCommandEvent& event);
		void OnSelectInvert(wxCommandEvent& event);
		void OnSetRoiType(wxCommandEvent& evt);

		void OnMenuClick(wxCommandEvent& event);
		
		void ManagePanels(PanelStates state);
		void deleteWorker();

		void OnEditCtrlCommand(wxCommandEvent& event);
		void OnStartSelect(wxCommandEvent& event);

		void OnControlBtns(wxCommandEvent& event);
		void OnKeybordBtns(wxCommandEvent& event);

		void OnTextInput(wxCommandEvent& event);

		void OnAddCtrlCmd(wxCommandEvent& event);
		void OnModeSelection(wxCommandEvent& event);
		void OnContextMenu(wxContextMenuEvent& event, wxPoint mousePosition);

		void OnSelectedFile(wxCommandEvent& event);

		void OnLoopBtn(wxCommandEvent& event);
		void OnRunCmdTimer(wxTimerEvent& event);

		void OnScreenshotTimer(wxTimerEvent& event);

		void OnSave(wxCommandEvent& event);
		void OnQuit(wxCommandEvent & event);
		void OnClose(wxCloseEvent& event);

		DECLARE_EVENT_TABLE()
};

//====================================================================

inline void RecorderPlayerKM::OnUpdateFileDropdown(wxCommandEvent& event)
{
	int n=m_fileDropDown->FindString(event.GetString());
	if(wxNOT_FOUND!=n){
		m_fileDropDown->Delete(n);
	}	
}

//--------------------------------------------------------------------

inline void RecorderPlayerKM::OnModeSelection(wxCommandEvent& event)
{
	OnModeSelection(static_cast<CommandInputMode>(m_inputModeDropdown->GetSelection()));
}

//--------------------------------------------------------------------

inline void RecorderPlayerKM::OnContextMenu(wxContextMenuEvent& event, wxPoint mousePosition)
{
	m_click=mousePosition-m_topLeftCorner;

	mkMenu(m_currentWindoRect.Contains(mousePosition), m_fullMenu);
}

//--------------------------------------------------------------------

inline void RecorderPlayerKM::SetCurrentWindow()
{
	SetCurrentWindow(m_currentWindow.c_str());
}

//--------------------------------------------------------------------

inline void RecorderPlayerKM::OnUpdateROI(wxCommandEvent& event)
{
	m_editCtrlCmdPopup->Dismiss();
	m_fullMenu=false;
	windowLevelInput(event.GetString());
}

//--------------------------------------------------------------------

inline void RecorderPlayerKM::postEvent(wxEventType commandEventType, int id)
{
	wxCommandEvent event(commandEventType, id);
	wxPostEvent(this, event);
}

//--------------------------------------------------------------------

template<typename T=InputCommand>
void RecorderPlayerKM::addCommand(BaseCommand* cmd)
{
	if(cmd){
		m_scrolledWindow->addCommand<T>(cmd, m_indentation);
		addCommand();
	}
}

template<>
inline void RecorderPlayerKM::addCommand<CtrlCommand>(BaseCommand* cmd)
{
	if(cmd){
		m_scrolledWindow->addCommand<CtrlCommand>(cmd, m_indentation);
		addCommand();
	}
}

//--------------------------------------------------------------------

inline void RecorderPlayerKM::OnSelectInvert(wxCommandEvent& event)
{
	if(event.GetId()==WX::SELECT_ALL){
		m_selectAllCheck->SetValue(false);
		m_scrolledWindow->selectAll();
	}
	else{
		m_invertCheck->SetValue(false);
		m_scrolledWindow->invert();
	}
}

//--------------------------------------------------------------------

inline void RecorderPlayerKM::OnQuit(wxCommandEvent& event)
{
	if(SaveChangesData()){
		return;
	}
	event.Skip();
	Destroy();
}

//--------------------------------------------------------------------

inline void RecorderPlayerKM::OnClose(wxCloseEvent& event)
{
	if(SaveChangesData()){
		return;
	}
	event.Skip();
	Destroy();
}

//--------------------------------------------------------------------

template<typename FUNC>
void RecorderPlayerKM::windowInputScreenshot(const char* windowName, const char* outputImage, FUNC cbk)
{
	if(m_fullFunctionality==SystemStatus::PARTIAL){
		return;
	}

	if(!windowExists(windowName)){
		wxMsgBox("Window with name \"%s\" not exists.", windowName);
		return;
	}

	getWindowROI(windowName);

	ManagePanels(PanelStates::Playing);

	bool taken=takeScreenshot(windowName, outputImage);

	ManagePanels(PanelStates::Recording);

	if(taken){
		cbk(outputImage);
	}
	else{
		removeImage(outputImage);
		wxMsgBox("Window with name \"%s\" not exists.", windowName);
	}
}

//====================================================================

#endif
