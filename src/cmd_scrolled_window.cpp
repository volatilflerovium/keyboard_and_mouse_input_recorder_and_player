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
#include "cmd_scrolled_window.h"

#include "event_definitions.h"
#include "command_wrapper.h"
#include "input_command.h"
#include "command_parser.h"
#include "enumerations.h"
#include "cstr_split.h"

#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/valnum.h>

//====================================================================

CmdScrolledWindow::CmdScrolledWindow(wxWindow* parent, int Id, wxPoint Point, wxSize wSize)
:wxScrolledWindow(parent, Id, Point, wSize)
, m_previousPanel(nullptr)
, m_width(wSize.GetWidth())
, m_height(0)
, m_loopStartAt(0)
, m_commandCount(0)
, m_init(false)
{
	SetBackgroundColour(wxColour("#FFFFFF"));
}

//--------------------------------------------------------------------

CmdScrolledWindow::~CmdScrolledWindow()
{
	for(BasePanel* panelPtr : m_cmdViewList){
		wxDELETE(panelPtr);
	}
}

//--------------------------------------------------------------------

BEGIN_EVENT_TABLE(CmdScrolledWindow, wxScrolledWindow)
	EVT_MENU(WX::DELETE_CMD, CmdScrolledWindow::DeleteCmd)
END_EVENT_TABLE()

//--------------------------------------------------------------------

void CmdScrolledWindow::DeleteCmd(wxCommandEvent& event)
{
	wxMessageDialog confirm(
		this,
		wxT("Do you want to delete the selected command?"),
		wxT(""),
		wxYES_NO|wxCENTRE|wxICON_WARNING
	);

	if(wxID_NO==confirm.ShowModal()){
		return;
	}

	BasePanel* basePanelPtr=nullptr;
	if(m_cmdViewList.size()>0){
		Scroll(0, 0);
		int height=0;
		int scrollPosition=0;
		int hPosition=0;
		bool found=false;
		std::list<BasePanel*>::iterator it=m_cmdViewList.begin();
		while(it!=m_cmdViewList.end()){
			basePanelPtr=*it;
			if(basePanelPtr->isSelected()){
				CommandPanel::s_lastSelected=nullptr;
				height=-1*basePanelPtr->getHeight();
				
				it=m_cmdViewList.erase(it);
				delete(basePanelPtr);

				found=true;
				scrollPosition=hPosition;
				wxCommandEvent cmdEvent(wxEVT_CUSTOM_EVENT, EvtID::CHANGES_MADE);
				wxPostEvent(this, cmdEvent);
				if(it==m_cmdViewList.end()){
					break;
				}
				basePanelPtr=*it;
			}
			
			if(found){
				basePanelPtr->SetPosition(wxPoint(0, hPosition));
			}
		
			hPosition+=basePanelPtr->getHeight();
			
			++it;
		}

		updateScroll(height);
		Scroll(0, scrollPosition/10);

		if(found){
			m_commandCount--;
			wxCommandEvent cmdEvent(wxEVT_CUSTOM_EVENT, EvtID::CMD_COUNT_UPDATED);
			wxPostEvent(this, cmdEvent);
		}
	}
}

//--------------------------------------------------------------------

void CmdScrolledWindow::updateScroll(int height)
{
	m_height+=height;

	const int stepY=10;
	int hy=1+std::floor(m_height/stepY);

	SetScrollbars(1, stepY, 0, hy, 0, 0, true);
	Scroll(0, hy);
}

//--------------------------------------------------------------------

void CmdScrolledWindow::addLoop(int times)
{
	Scroll(0, 0);
	LoopPanel* loop=new LoopPanel(this, m_height, m_width, times);

	addCommandPanel(loop);
	m_loopStartAt=m_cmdViewList.size();
}

//--------------------------------------------------------------------

void CmdScrolledWindow::closeLoop()
{
	int length=m_cmdViewList.size()-m_loopStartAt;
	if(length>0 && m_loopStartAt>-1){
		Scroll(0, 0);
		BasePanel* closeLoop=new CloseLoopPanel(this, m_height, m_width);

		addCommandPanel(closeLoop);
		m_loopStartAt=-1;
	}
	else{
		removeLast();
	}
}

//----------------------------------------------------------------------

void CmdScrolledWindow::selectAll()
{
	for(BasePanel* panelPtr : m_cmdViewList){
		panelPtr->enableCommand(true);
	}
}

//----------------------------------------------------------------------

void CmdScrolledWindow::invert()
{
	for(BasePanel* panelPtr : m_cmdViewList){
		panelPtr->enableCommand(!panelPtr->isEnabled());
	}
}

//----------------------------------------------------------------------

void CmdScrolledWindow::removeLast()
{
	BasePanel* basePanelPtr=nullptr;
	if(m_cmdViewList.size()>0){
		std::list<BasePanel*>::iterator it=--m_cmdViewList.end();
		basePanelPtr=*it;
		m_cmdViewList.pop_back();

		int height=-1*basePanelPtr->getHeight();

		updateScroll(height);
		
		delete(basePanelPtr);
	}
}

//----------------------------------------------------------------------

void CmdScrolledWindow::clear()
{
	m_height=0;
	for(BasePanel* panelPtr : m_cmdViewList){
		wxDELETE(panelPtr);
	}

	CommandPanel::s_lastSelected=nullptr;

	m_cmdViewList.clear();

	SetScrollbars(1, 1, 0, 0, 0, 0);
	Scroll(0, 0);
	m_commandCount=0;
}

//----------------------------------------------------------------------

bool CmdScrolledWindow::getCmd(BaseCommand*& cmdPtr)
{
	static int times=0;
	static typename std::list<BasePanel*>::iterator it;
	if(!m_init){
		m_init=true;
		m_dataIt=m_cmdViewList.begin();
		it=m_dataIt;
		times=0;
		m_previousPanel=nullptr;
	}
	
	BasePanel* currentDataPtr=nullptr;
	while(m_dataIt!=m_cmdViewList.end()){
		currentDataPtr=*m_dataIt;
		if(currentDataPtr->isPanel(PanelType::COMMAND)){
			if(m_previousPanel){
				m_previousPanel->SetBackgroundColour(wxColour("#FFFFFF"));
			}
			cmdPtr=currentDataPtr->getCommand();
			m_previousPanel=(*m_dataIt);
			m_previousPanel->SetBackgroundColour(wxColour("#49d470"));
			++m_dataIt;
			return true;
		}
		else if(currentDataPtr->isPanel(PanelType::OPEN_LOOP)){
			times=currentDataPtr->getTimes();
			it=m_dataIt;
		}
		else if(currentDataPtr->isPanel(PanelType::CLOSE_LOOP)){
			if(--times>0){
				m_dataIt=it;
			}
		}
		++m_dataIt;
	}
	if(m_previousPanel){
		m_previousPanel->SetBackgroundColour(wxColour("#FFFFFF"));
	}
	cmdPtr=nullptr;
	m_init=false;

	return false;
}

//--------------------------------------------------------------------

void CmdScrolledWindow::lastCommandFailed()
{
	if(m_previousPanel){
		m_previousPanel->enableStatus();
	}
}

//----------------------------------------------------------------------

void CmdScrolledWindow::reset()
{
	m_init=false;
	m_previousPanel=nullptr;
	for(auto panelPtr : m_cmdViewList){
		panelPtr->reset();
	}
}

//--------------------------------------------------------------------

bool CmdScrolledWindow::getCommand(BaseCommand*& cmdPtr)
{
	if(!m_init){
		m_dataIt=m_cmdViewList.begin();
	}

	cmdPtr=nullptr;
	m_init=(m_dataIt!=m_cmdViewList.end());
	if(m_init){
		if(!(*m_dataIt)->isPanel(PanelType::COMMAND)){
			++m_dataIt;
		}
		
		if(m_dataIt==m_cmdViewList.end()){
			m_init=false;
		}
		else{
			cmdPtr=(*m_dataIt)->getCommand();//)m_baseCommandPtr;
			++m_dataIt;
		}
	}
	
	return m_init;
}

//--------------------------------------------------------------------

bool CmdScrolledWindow::getCommand(BaseCommand*& cmdPtr, const CmdScrolledWindow::PlayMode mode)
{
	typedef bool(CmdScrolledWindow::* Func)(BaseCommand*&);

	Func getting=&CmdScrolledWindow::getCmd;

	if(mode==CmdScrolledWindow::PlayMode::DEMO){
		getting=&CmdScrolledWindow::getCommand;
	}

	while((this->*getting)(cmdPtr)){
		if(!cmdPtr->isActive()){	
			continue;
		}
		return true;
		break;
	}
	return false;
}

//----------------------------------------------------------------------

bool CmdScrolledWindow::swap(bool downSwap)
{
	bool doSwap=false;	
	int height=0;

	std::list<BasePanel*>::iterator prev=m_cmdViewList.begin();	
	std::list<BasePanel*>::iterator next=m_cmdViewList.begin();

	while(++next!=m_cmdViewList.end()){
		if(downSwap && (*prev)->isSelected()){
			doSwap=true;
			break;
		}
		if(!downSwap && (*next)->isSelected()){
			doSwap=true;
			break;
		}
		height+=(*prev)->getHeight();
		prev=next;
	}

	if(doSwap){
		Scroll(0, 0);

		BasePanel* basePanelPtr=*prev;
		*prev=*next;
		*next=basePanelPtr;

		auto fixHeight=[](BasePanel* bPanelPtr, int height){
			bPanelPtr->SetPosition(wxPoint(0, height));
			height+=bPanelPtr->getHeight();
			return height;
		};

		int height2=height;

		height=fixHeight(*prev, height);
		fixHeight(*next, height);

		Scroll(0, height2/10);

		if((*prev)->isPanel(PanelType::OPEN_LOOP)){
			(*next)->doIndentation(true);
			m_loopStartAt--;
		}
		else if((*prev)->isPanel(PanelType::CLOSE_LOOP)){
			(*next)->doIndentation(false);
		}
		else if((*next)->isPanel(PanelType::OPEN_LOOP)){
			(*prev)->doIndentation(false);
			m_loopStartAt++;
		}
		else if((*next)->isPanel(PanelType::CLOSE_LOOP)){
			(*prev)->doIndentation(true);
		}		
	}

	return doSwap;
}

//--------------------------------------------------------------------

bool CmdScrolledWindow::saveData(const char* fileName)
{
	std::fstream fileData(fileName, std::ios::out | std::ios::trunc);
	if(fileData.is_open()){
		for(auto dataHolder : m_cmdViewList){
			if(dataHolder->isPanel(PanelType::COMMAND)){
				// DO NOT DELETE this comment: implement null object here 
				dataHolder->getCommand()->print(fileData);
			}
			else if(dataHolder->isPanel(PanelType::OPEN_LOOP)){
				fileData<<"loop:"<<dataHolder->getTimes()<<"\n";
			}
			else if(dataHolder->isPanel(PanelType::CLOSE_LOOP)){
				fileData<<"end_loop\n";
			}
		}
	}
	bool a=fileData.is_open();
	fileData.close();
	return a;
}

//--------------------------------------------------------------------

bool CmdScrolledWindow::loadDataFile(const char* fileName)
{
	bool result=true;
	std::ifstream commandFiles;
	commandFiles.open(getFilePath(fileName), std::ifstream::in);
	if(commandFiles.is_open()){

		bool indentation=false;
		std::string commandLine;
		while(std::getline(commandFiles, commandLine)){
			try{
				if(commandLine.length()==0){
					continue;
				}

				size_t pos=commandLine.find("loop:");

				if(pos==0){
					CstrSplit<2> parts(commandLine.c_str(), ":");
					addLoop(std::atoi(parts[1]));
					indentation=true;
					continue;
				}

				pos=commandLine.find("end_loop");
				if(pos==0){
					closeLoop();
					indentation=false;
					continue;
				}

				ParserBuilder(this, indentation, commandLine);
			}
			catch(...){//const std::exception& e){
				result=false;
				break;
			}
		}
		commandFiles.close();
	}
	return result;	
}

//====================================================================
