/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* CmdScrolledWindow class                          				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    28-10-2024                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef CMD_SCROLLED_WINDOW_H
#define CMD_SCROLLED_WINDOW_H

#include "command_wrapper.h"

#include <wx/wx.h>
#include <wx/scrolwin.h>
#include <list>

class BasePanel;
class BaseCommand;
class InputCommand;
class MouseBtnCommand;
class CtrlCommand;

//====================================================================

template<typename T>
struct HelperBuilder
{
	typedef InputCommandWrapper Wrapper;
};

template<>
struct HelperBuilder<CtrlCommand>
{
	typedef ControlCommandWrapper Wrapper;
};

template<>
struct HelperBuilder<MouseBtnCommand>
{
	typedef MouseBtnCmdWrapper Wrapper;
};

//====================================================================

class CmdScrolledWindow : public wxScrolledWindow
{
	public:
		enum PlayMode
		{
			DEMO,
			NORMAL
		};

	public:
		CmdScrolledWindow(wxWindow* parent, int Id, wxPoint Point, wxSize wSize);

		virtual ~CmdScrolledWindow();

		template<typename T>
		void addCommand(T* cmd, bool indentation);

		void addLoop(int times);
		void closeLoop();
		void selectAll();
		void invert();
		void removeLast();
		void clear();

		bool getCommand(BaseCommand*& cmdPtr, const CmdScrolledWindow::PlayMode mode);
		void advance2End(size_t idx);

		size_t size() const;
		unsigned int getCommandCount() const;

		void reset();
		bool swapUp();
		bool swapDown();

		bool saveData(const char* fileName);
		bool loadDataFile(const char* fileName);

		void lastCommandFailed();

	private:
		std::list<BasePanel*> m_cmdViewList;
		typename std::list<BasePanel*>::iterator m_dataIt;

		BaseCommand* m_currentRunningCmd;
		BasePanel* m_previousPanel;
		const int m_width;
		int m_height;
		int m_loopStartAt;
		int m_commandCount;
		bool m_init;

		bool getCmd(BaseCommand*& cmdPtr);
		bool getCommand(BaseCommand*& cmdPtr);
		bool swap(bool downSwap);
		void addCommandPanel(BasePanel* cmd);
		void updateScroll(int height);

		void DeleteCmd(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------

inline unsigned int CmdScrolledWindow::getCommandCount() const
{
	return m_commandCount;
}

//----------------------------------------------------------------------

inline bool CmdScrolledWindow::swapUp()
{
	return swap(false);
}

//----------------------------------------------------------------------

inline bool CmdScrolledWindow::swapDown()
{
	return swap(true);
}

//--------------------------------------------------------------------

inline size_t CmdScrolledWindow::size() const
{
	return m_cmdViewList.size();
}

//--------------------------------------------------------------------

inline void CmdScrolledWindow::advance2End(size_t idx)
{
	BaseCommand* cmdPtr;
	for(size_t k=0; k<size(); k++){
		if(k==idx){
			break;
		}
		getCommand(cmdPtr);
	}
}


//--------------------------------------------------------------------

inline void CmdScrolledWindow::addCommandPanel(BasePanel* panelPtr)
{
	updateScroll(panelPtr->getHeight());
	m_cmdViewList.push_back(panelPtr);
}

//--------------------------------------------------------------------

template<typename T>
void CmdScrolledWindow::addCommand(T* cmd, bool indentation)
{
	Scroll(0, 0);

	BasePanel* panelPtr=new typename HelperBuilder<T>::Wrapper(this, m_height, m_width, cmd);

	panelPtr->init(indentation);

	addCommandPanel(panelPtr);
	m_commandCount++;// only commands count, loops do not count
}

//====================================================================

#endif
