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
#ifndef COMMAND_WRAPPER_H
#define COMMAND_WRAPPER_H

#include "wrapper_panel.h"
#include "wx_textctrl.h"

#include <wx/wx.h>
#include <wx/valnum.h>

//====================================================================

class BaseCommand;
class InputCommand;
class CtrlCommand;
class ResultPopup;
class MouseBtnCommand;

//====================================================================

enum class PanelType
{
	COMMAND,
	OPEN_LOOP,
	CLOSE_LOOP,
};

//====================================================================

class BasePanel : public wxPanel
{
	public:
		virtual ~BasePanel()=default;

		virtual void init(bool indentation=false)=0;

		virtual void enableCommand(bool enable)=0;

		virtual bool isEnabled() const=0;

		virtual void doIndentation(bool indentation)=0;

		virtual bool isSelected() const=0;

		virtual BaseCommand* getCommand()=0;

		virtual int getTimes() const=0;

		virtual bool isPanel(PanelType panelType) const=0;

		virtual int getHeight() const=0;
		
		virtual void reset()
		{}

		virtual void enableStatus()
		{}
};

//====================================================================

class LoopPanel : public BaseWrapperPanel<BasePanel>
{
	public:
		LoopPanel(wxWindow* parent, uint posY, uint width, int times);

		virtual ~LoopPanel()=default;

		virtual void init(bool indentation=false)
		{}

		virtual void enableCommand(bool enable)
		{}

		virtual bool isEnabled() const
		{
			return true;
		}

		virtual bool isSelected() const
		{
			return false;
		}

		virtual bool isPanel(PanelType panelType) const
		{
			return PanelType::OPEN_LOOP==panelType;
		}

		virtual void doIndentation(bool indentation) override
		{}

		virtual int getTimes() const
		{
			return m_times;
		}

		void setTimes(int times)
		{
			m_times=times;
		}

		virtual BaseCommand* getCommand()
		{
			return nullptr;
		}

		virtual int getHeight() const
		{
			return m_height;
		}

	private:
		WX_TextCtrl* m_loopInput;
		int m_times{0};
};

//====================================================================

class CloseLoopPanel : public BaseWrapperPanel<BasePanel>
{
	public:
		CloseLoopPanel(wxWindow* parent, uint posY, uint width);

		virtual ~CloseLoopPanel()=default;

		virtual void init(bool indentation=false)
		{}

		virtual void enableCommand(bool enable)
		{}

		virtual bool isEnabled() const
		{
			return true;
		}

		virtual bool isSelected() const
		{
			return false;
		}

		virtual bool isPanel(PanelType panelType) const
		{
			return PanelType::CLOSE_LOOP==panelType;
		}
		
		virtual void doIndentation(bool indentation) override
		{}

		virtual int getTimes() const override
		{
			return 0;
		}

		virtual BaseCommand* getCommand()
		{
			return nullptr;
		}

		virtual int getHeight() const
		{
			return m_height;
		}
};

//====================================================================
//====================================================================

struct CmdSettingData : public SettingData
{
	static constexpr int TOP_MARGIN_PADDING=10;
	static constexpr int MARGIN_WIDTH=2;
	static constexpr int WX_ID=WX::DELETE_CMD;
};

//--------------------------------------------------------------------

class CommandPanel : public WrapperPanel<CmdSettingData, BasePanel>
{
	public:
		CommandPanel(wxWindow* parent, uint posY, uint width)
		: WrapperPanel<CmdSettingData, BasePanel>(parent, posY, width)
		{
		}

		virtual ~CommandPanel()=default;

		virtual void init(bool indentation);

		virtual void enableCommand(bool enable);

		virtual bool isEnabled() const
		{
			return m_enableCmdCheck->GetValue();
		}

		virtual bool isPanel(PanelType panelType) const
		{
			return PanelType::COMMAND==panelType;
		}

		virtual void doIndentation(bool indentation) override;

		virtual int getTimes() const override
		{
			return 0;
		}

		virtual int getHeight() const
		{
			return m_height;
		}

		virtual void enableStatus() override;
		
		virtual void reset() override
		{
			m_statusBtn->SetBackgroundColour(wxColour("#FFFFFF"));
			SetBackgroundColour(wxColour("#FFFFFF"));
		}

	protected:
		wxBoxSizer* m_mainCol;
		wxBoxSizer* m_paddingCol;
		wxBoxSizer* m_sizerBody;
		wxButton* m_statusBtn;

		wxCheckBox* m_enableCmdCheck;
		WX_TextCtrl* m_description;
		WX_TextCtrl* m_timeoutInput;

		bool m_isIndented{false};

		static constexpr int c_padding=30;

		virtual void setTimeoutCtrl()=0;
		virtual void OnCheck(wxCommandEvent& event);
		virtual void OnCheckStatus(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()

	friend class CmdScrolledWindow;
};

//====================================================================

class InputCommandWrapper : public CommandPanel
{
	public:
		InputCommandWrapper(wxWindow* parent, uint posY, uint width, InputCommand* cmd);

		virtual ~InputCommandWrapper();

		virtual void init(bool indentation=false);

		virtual BaseCommand* getCommand() override;

	protected:
		InputCommand* m_cmdPtr;

		virtual void setTimeoutCtrl() override;

	DECLARE_EVENT_TABLE()
};

//====================================================================

class MouseBtnCmdWrapper : public InputCommandWrapper
{
	public:
		MouseBtnCmdWrapper(wxWindow* parent, uint posY, uint width, MouseBtnCommand* cmd);

		virtual ~MouseBtnCmdWrapper()=default;

		virtual void init(bool indentation=false);

	private:
		WX_TextCtrl* m_pressForMsInput;

	DECLARE_EVENT_TABLE()
};

//====================================================================


class ControlCommandWrapper : public CommandPanel
{
	public:
		ControlCommandWrapper(wxWindow* parent, uint posY, uint width, CtrlCommand* cmdPtr);

		virtual ~ControlCommandWrapper();

		virtual void init(bool indentation=false);

		void updateTimeout(int timeout)
		{
			m_timeoutInput->ChangeValue(wxString::Format("%i", timeout));
		}

		virtual BaseCommand* getCommand() override;

	protected:
		virtual void setTimeoutCtrl() override;
		virtual void OnCheckStatus(wxCommandEvent& event) override;
		virtual void mkContextMenu();

	private:
		CtrlCommand* m_cmdPtr;
		ResultPopup* m_imgCtrlViewrPtr;

		DECLARE_EVENT_TABLE()
};

//====================================================================

#endif
