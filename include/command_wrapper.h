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

class CommandPanel : public WrapperPanel<CommandPanel, 10, 2, WX::DELETE_CMD, BasePanel>
{
	public:
		CommandPanel(wxWindow* parent, uint posY, uint width, BaseCommand* cmd)
		:WrapperPanel<CommandPanel, 10, 2, WX::DELETE_CMD, BasePanel>(parent, posY, width)
		{
			m_baseCommandPtr=cmd;
		}

		virtual ~CommandPanel();

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

		virtual BaseCommand* getCommand()
		{
			return m_baseCommandPtr;
		}

		virtual int getHeight() const
		{
			return m_height;
		}

		virtual void enableStatus() override;

	protected:
		BaseCommand* m_baseCommandPtr{nullptr};
		wxBoxSizer* m_mainCol;
		wxBoxSizer* m_paddingCol;
		wxBoxSizer* m_sizerBody;
		wxButton* m_statusBtn;

		wxCheckBox* m_enableCmdCheck;
		WX_TextCtrl* m_description;
		WX_TextCtrl* m_timeoutInput;

		bool m_isIndented{false};

		virtual void setSelected() override
		{
			s_lastSelected=this;
		}

		static constexpr int c_padding=30;

		virtual void OnCheck(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()

	friend class ExtScrolledWindow;
};

//====================================================================

class InputCommandWrapper : public CommandPanel
{
	public:
		InputCommandWrapper(wxWindow* parent, uint posY, uint width, BaseCommand* cmd);

		virtual ~InputCommandWrapper()=default;

		virtual void init(bool indentation=false);
};

//====================================================================

class ControlCommandWrapper : public CommandPanel
{
	public:
		ControlCommandWrapper(wxWindow* parent, uint posY, uint width, BaseCommand* cmd);

		virtual ~ControlCommandWrapper()=default;

		virtual void init(bool indentation=false);

		virtual void enableCommand(bool enable);

		void updateTimeout(int timeout)
		{
			m_timeoutInput->ChangeValue(wxString::Format("%i", timeout));
		}

	private:
		wxButton* m_edit;

		DECLARE_EVENT_TABLE()
};

//====================================================================

#endif
