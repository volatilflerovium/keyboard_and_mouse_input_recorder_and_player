/*********************************************************************
* class AddCmdPopup                                                  *
* class EditCtrlCmdPopup                                             *
* class WindowPreview                                                *
* class FileListPopup                                                *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _DEDICATED_POPUP_H
#define _DEDICATED_POPUP_H

#include "extended_popup.h"
#include "utilities.h"

#include <wx/wx.h>

class CtrlCommand;
class WX_TextCtrl;
class wxSpinCtrl;
class ImagePanel;

//====================================================================

class AddCmdPopup : public ExtendedPopup
{
	public:
		AddCmdPopup(wxWindow* parent, const char* title);

		virtual ~AddCmdPopup()=default;

		void loadRoi(const char* imgName, const char* roi, const char* windowName);

	protected:
		std::string m_imgName;
		std::string m_roi;
		std::string m_windowName;
		wxButton* m_cancelBtn;
		wxButton* m_summitBtn;
		wxSpinCtrl* m_thresholdInput;

		wxSpinCtrl* m_timeoutInput;
		wxRadioBox* m_ctrlCmdModeSetRadio;
		ImagePanel* m_previewPanel;
		wxCheckBox* m_strictRunCheck;

		wxStaticText* m_instructions;
		wxStaticText* m_timeoutTxt;
		wxStaticText* m_thresholdTxt;
		wxStaticText* m_previewTxt;

		AddCmdPopup(wxWindow* parent, const char* title, bool)
		:ExtendedPopup(parent, title)
		{
		}

		virtual void init();
		virtual void layout();
		virtual void removeImg();
		virtual void onCancel();

		void OnAddCtrlCmd(wxCommandEvent& event);
	
	private:		
		DECLARE_EVENT_TABLE()
};

//--------------------------------------------------------------------

inline void AddCmdPopup::onCancel()
{
	Dismiss();
	removeImg();
}

//====================================================================

class EditCtrlCmdPopup : public AddCmdPopup
{
	public:
		EditCtrlCmdPopup(wxWindow* parent, const char* title);

		virtual ~EditCtrlCmdPopup()=default;

		bool loadCommand(CtrlCommand* ctrlCmdPtr);

		void loadNewRoi(const char* imgName, const char* roi);

		bool isEditing()// use this to check if the new selection if for replacing command image
		{
			return m_ctrlCmdPtr!=nullptr;
		}

		void setOnCancel(std::function<void(void)> cbk)
		{
			m_onCancelCbk=cbk;
		}

	protected:
		std::function<void(void)> m_onCancelCbk;
		CtrlCommand* m_ctrlCmdPtr;
		wxRadioBox* m_swapScreenshotRadio;
		wxButton* m_updateScreenshotBtn;

		enum USE_ROI
		{
			OLD=0,
			NEW,
		};

		virtual void init() override;
		virtual void layout() override;
		virtual void onCancel() override;

		void OnUpdateCmd(wxCommandEvent& event);
		void OnSwapScreenshot(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
};

//--------------------------------------------------------------------

inline void EditCtrlCmdPopup::loadNewRoi(const char* imgName, const char* roi)
{
	AddCmdPopup::loadRoi(imgName, roi, m_windowName.c_str());
	m_swapScreenshotRadio->SetSelection(1);
	m_swapScreenshotRadio->Enable();
	Layout();
}

//--------------------------------------------------------------------

inline void EditCtrlCmdPopup::onCancel()
{
	AddCmdPopup::onCancel();
	m_ctrlCmdPtr=nullptr;
	if(m_onCancelCbk){
		m_onCancelCbk();
	}
}

//====================================================================
/* We derived from BasicPopup to avoid multiple set of duplicated functionality:
 * onClose/OnDismiss/CancelBtn by forcing user to close explicitly the window
 * */
class WindowPreview : public BasicPopup
{
	public:
		WindowPreview(wxWindow* parent, const char* title);

		virtual ~WindowPreview()=default;

		void loadImage(const char* imagePath);

		void bindCancelBtn(std::function<void(wxCommandEvent& event)> cbk)
		{
			m_cancelBtn->Bind(wxEVT_BUTTON, cbk);
		}

		void bindOkBtn(std::function<void(wxCommandEvent& event)> cbk)
		{
			m_okBtn->Bind(wxEVT_BUTTON, cbk);
		}

	private:
		wxButton* m_cancelBtn;
		wxButton* m_okBtn;
		ImagePanel* m_previewPanel;
		wxBoxSizer* m_imageHolder;
		wxBoxSizer* m_bodySizer;
		wxBoxSizer* m_boxWrapper;
		wxBoxSizer* m_btnsRow;
};

//====================================================================

class FileScrolledWindow;

class FileListPopup : public WX_Popup
{
	public:
		FileListPopup(wxWindow* parent, const char* title);

		virtual ~FileListPopup()=default;

		void setOpenFile(const char* fileName)
		{
			m_openFile=fileName;
		}

	private:
		std::string m_openFile;
		FileScrolledWindow* m_fileScrolledWindow;

		wxButton* m_closeBtn;
		wxBoxSizer* m_boxWrapper;

		virtual void OnPopup() override;
		virtual void OnDismiss() override;
};

//====================================================================

#endif
