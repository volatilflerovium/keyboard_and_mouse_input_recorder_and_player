/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class ExitCode                                                     *
* struct MouseCmdExitPosition                                        *
* class WindowOffset                                                 *
* class BaseCommand                                                  *
* class InputCommand                                                 *
* class TextCommand                                                  *
* class LineCommand                                                  *
* class KeyCommad                                                    *
* class UnicodeCommand                                               *
* class ShortcutCommand                                              *
* class MoveMouseCommand                                             *
* class MouseLeftBtnCommand                                          *
* class MouseRightBtnCommand                                         *
* class MouseSelectCommand                                           *
* class MouseSelectCommand2                                          *
* class CtrlCommand                                                  *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "input_command.h"
#include "hid_manager.h"
#include "ImageDiff_Lib/simple_image_difference.h"
#include "utilities.h"
#include "debug_utils.h"

#include <wx/wx.h>

int MouseCmdExitPosition::s_x=0;
int MouseCmdExitPosition::s_y=0;

void MouseCmdExitPosition::setExitPosition()
{
	s_x=wxGetMousePosition().x;
	s_y=wxGetMousePosition().y;
}

//====================================================================

const char* const ExitCode::ExitCodeVerbose[ExitCode::TOTAL_MSG]={
	"OK",                          //=0,
	"Failed",                      //FAILED=1<<1,
	"Timeout",                     //TIMEOUT=1<<2,
	"Control image missing",       //"BASE_IMAGE_MISSING=1<<3,
	"Target window is closed",     //TARGET_WINDOW_CLOSED=1<<4,
	"Image corruption",            //CV_EXCEPTION=1<<5,
	"The input position for the command\nwas out of the visible screen",//OUT_OF_BOUND=1<<6,// when pointer is trying to get to a position outside of the screen
	"System error"                 //SYSTEM_FAILED=1<<7,
	"Unknown"
};

//====================================================================

static SimpleImageDifference* GetImageDifference()
{
	static SimpleImageDifference s_simpleImageDifference;

	return &s_simpleImageDifference;
}

//====================================================================

void MouseLeftClick(int x, int y)
{
	s_MouseEmulator->go2Position(x, y, [](int& pX, int& pY){
		wxPoint mousePosition=wxGetMousePosition();
		pX=mousePosition.x;
		pY=mousePosition.y;
	});

	s_MouseEmulator->clickLeftBtn();
}

//====================================================================

int ExitCode::ln(int x)
{
	int n=0;
	while(x>1){
		x=x>>1;
		n++;
	}
	if(n<ExitCode::LAST){
		return n;
	}
	return ExitCode::UNKNOWN;
}

//====================================================================

bool WindowOffset::isTargetValid(int x, int y)
{
	if(isAbsolute()){
		m_absoluteX=x;
		m_absoluteY=y;
		return true;
	}

	if(::windowExists(m_windowName.c_str())){
		WindowRect rect=getWindowRect(m_windowName.c_str(), true);
		m_absoluteX=x+rect.m_x;
		m_absoluteY=y+rect.m_y;
		return rect.inside(x, y);
	}
	return false;
}

//====================================================================

TextCommand::TextCommand(const char* description, int wait, const std::string& text)
:InputCommand(description, wait)
, m_text(text)
{
	m_cmd=[this](){
		s_KeyboardEmulator->inputText(m_text.c_str());
	};

	int ID=static_cast<int>(CommandTypes::KeyboardText);

	m_strCmd=[this, ID](){
		return ToString2(ID, m_description, m_run, m_text);
	};
}

//====================================================================

LineCommand::LineCommand(const char* description, int wait, const std::string& line)
:InputCommand(description, wait)
, m_line(line)
{
	m_cmd=[this](){
		s_KeyboardEmulator->inputLine(m_line.c_str());
	};

	int ID=static_cast<int>(CommandTypes::KeyboardLine);

	m_strCmd=[this, ID](){
		return ToString2(ID, m_description, m_run, m_line);
	};
}

//====================================================================

KeyCommad::KeyCommad(const char* description, int wait, SPKEYS keyCode)
:InputCommand(description, wait)
, m_keyCode(keyCode)
{
	m_cmd=[this](){
		s_KeyboardEmulator->commandKey(m_keyCode);
	};

	int ID=static_cast<int>(CommandTypes::Keyboard);

	m_strCmd=[this, ID](){
		return ToString2(ID, m_description, m_run, int(m_keyCode));
	};
}

//====================================================================

UnicodeCommand::UnicodeCommand(const char* description, int wait, const std::string& codePoint)
:InputCommand(description, wait)
, m_codePoint(codePoint)
{
	m_cmd=[this](){
		s_KeyboardEmulator->unicodeCharacter(m_codePoint.c_str());
	};

	int ID=static_cast<int>(CommandTypes::Unicode);

	m_strCmd=[this, ID](){
		return ToString2(ID, m_description, m_run, m_codePoint);
	};
}

//====================================================================

ShortcutCommand::ShortcutCommand(const char* description, int wait, const std::string& shortcut)
:InputCommand(description, wait)
, m_shortcut(shortcut)
{
	ComboStringParser shortcutObj(m_shortcut);
	m_cmd=[shortcutObj](){
		s_KeyboardEmulator->shortcut(shortcutObj);
	};
	

	int ID=static_cast<int>(CommandTypes::Shortcut);

	m_strCmd=[this, ID](){
		return ToString2(ID, m_description, m_run, m_shortcut);
	};
}

//--------------------------------------------------------------------

ShortcutCommand* ShortcutCommand::Builder(const char* description, int wait, const char* shortcut)
{
	return new ShortcutCommand(description, wait, shortcut);
}

//====================================================================

MoveMouseCommand::MoveMouseCommand(const char* description, int wait, int x, int y, const char* windowName)
:InputCommand(description, wait)
, WindowOffset(windowName)
, m_x(x)
, m_y(y)
{
	m_cmd=[this](){
		m_statusCode=ExitCode::TARGET_WINDOW_CLOSED;
		if(windowExists()){
			m_statusCode=ExitCode::OUT_OF_BOUND;
			if(isTargetValid(m_x, m_y)){
				m_statusCode=ExitCode::OK;
				s_MouseEmulator->go2Position(m_absoluteX, m_absoluteY, [](int& pX, int& pY){
					wxPoint mousePosition=wxGetMousePosition();
					pX=mousePosition.x;
					pY=mousePosition.y;
				});
			}
		}
	};

	int ID=static_cast<int>(CommandTypes::MouseMove);

	m_strCmd=[this, ID](){
		return ToString2(ID, m_description, m_run, m_x, m_y, m_windowName);
	};
}

//====================================================================

MouseLeftBtnCommand::MouseLeftBtnCommand(const char* description, int wait, int x, int y, const char* windowName)
:InputCommand(description, wait)
, WindowOffset(windowName)
, m_x(x)
, m_y(y)
{
	m_cmd=[this](){
		m_statusCode=ExitCode::TARGET_WINDOW_CLOSED;
		if(windowExists()){
			m_statusCode=ExitCode::OUT_OF_BOUND;
			if(isTargetValid(m_x, m_y)){
				m_statusCode=ExitCode::OK;
				s_MouseEmulator->go2Position(m_absoluteX, m_absoluteY, [](int& pX, int& pY){
					wxPoint mousePosition=wxGetMousePosition();
					pX=mousePosition.x;
					pY=mousePosition.y;
				});

				s_MouseEmulator->clickLeftBtn();
			}
		}
		MouseCmdExitPosition::setExitPosition();
	};

	int ID=static_cast<int>(CommandTypes::MouseLeftBtn);

	m_strCmd=[this, ID](){
		return ToString2(ID, m_description, m_run, m_x, m_y, m_windowName);
	};
}

//====================================================================

MouseRightBtnCommand::MouseRightBtnCommand(const char* description, int wait, int x, int y, const char* windowName)
:InputCommand(description, wait)
, WindowOffset(windowName)
, m_x(x)
, m_y(y)
{
	m_cmd=[this](){
		m_statusCode=ExitCode::TARGET_WINDOW_CLOSED;
		if(windowExists()){
			m_statusCode=ExitCode::OUT_OF_BOUND;
			if(isTargetValid(m_x, m_y)){
				m_statusCode=ExitCode::OK;
				s_MouseEmulator->go2Position(m_absoluteX, m_absoluteY, [](int& pX, int& pY){
					wxPoint mousePosition=wxGetMousePosition();
					pX=mousePosition.x;
					pY=mousePosition.y;
				});

				s_MouseEmulator->clickRightBtn();
			}
		}
		MouseCmdExitPosition::setExitPosition();
	};

	int ID=static_cast<int>(CommandTypes::MouseRightBtn);

	m_strCmd=[this, ID](){
		return ToString2(ID, m_description, m_run, m_x, m_y, m_windowName);
	};
}

//====================================================================

MouseSelectCommand::MouseSelectCommand(const char* description, int wait, int posX, int posY, int width, int height, const char* windowName)
:InputCommand(description, wait)
, WindowOffset(windowName)
, m_posX(posX)
, m_posY(posY)
, m_width(width)
, m_height(height)
{
	m_cmd=[this](){
		m_statusCode=ExitCode::TARGET_WINDOW_CLOSED;
		if(windowExists()){
			m_statusCode=ExitCode::OUT_OF_BOUND;
			if(isTargetValid(m_posX, m_posY)){
				m_statusCode=ExitCode::OK;
				s_MouseEmulator->drag(m_absoluteX, m_absoluteY, m_width, m_height, [](int& pX, int& pY){
					wxPoint mousePosition=wxGetMousePosition();
					pX=mousePosition.x;
					pY=mousePosition.y;
				});
			}
		}
	};

	int ID=static_cast<int>(CommandTypes::MouseSelection);

	m_strCmd=[this, ID](){
		return ToString2(ID, m_description, m_run, m_posX, m_posY, m_width, m_height, m_windowName);
	};
}

MouseSelectCommand2::MouseSelectCommand2(const char* description, int wait, int endX, int endY, const char* windowName)
:InputCommand(description, wait)
, WindowOffset(windowName)
, m_endX(endX)
, m_endY(endY)
{
	/*
	 * Notice that if the current mouse changed is not the same as the
	 * position of it when the last mouse command exit (it was moved accidently)
	 * we might get different result. Therefore it would be better to keep
	 * tract of the mouse position according to the last mouse command executed.
	 * 
	 * */
	m_cmd=[this](){
		m_statusCode=ExitCode::TARGET_WINDOW_CLOSED;
		if(windowExists()){
			m_statusCode=ExitCode::OUT_OF_BOUND;
			if(isTargetValid(m_endX, m_endY)){
				m_statusCode=ExitCode::OK;

				wxPoint mousePosition=wxGetMousePosition();
				int startX=mousePosition.x;
				int startY=mousePosition.y;

				WindowRect rect=getWindowRect(m_windowName.c_str(), true);
				int endAbsoluteX=m_endX+rect.m_x;
				int endAbsoluteY=m_endY+rect.m_y;
				int width=endAbsoluteX-startX;
				int height=endAbsoluteY-startY;

				s_MouseEmulator->drag(startX, startY, width, height, [](int& pX, int& pY){
					wxPoint mousePosition=wxGetMousePosition();
					pX=mousePosition.x;
					pY=mousePosition.y;
				});
			}
		}
		MouseCmdExitPosition::setExitPosition();
	};

	int ID=static_cast<int>(CommandTypes::MouseSelection);

	m_strCmd=[this, ID](){
		return ToString2(ID, m_description, m_run, m_endX, m_endY, m_windowName);
	};
}

//====================================================================

CtrlCommand::CtrlCommand(const char* description, const std::string& baseImageName, const char* roiStr, const char* windowName, bool removeImg)
:BaseCommand(description)
, WindowOffset(windowName)
, m_baseImageName(baseImageName)
, m_roiStr(roiStr)
, m_tries(1)
, m_triesCount(0)
, m_threshold(240)
, m_sensitivity(100)
, m_similarity(true)
, m_strictRun(true)
, m_cleanImg(removeImg)
{
	m_cbk=[](){
		return true;
	};

	m_statusCode=ExitCode::OK;

	m_cmd=[this](){
		m_triesCount=0;
		if(imageExists(m_baseImageName)){
			GetImageDifference()->loadBaseImage(getImgPath(m_baseImageName).c_str());
		}
		else{
			m_triesCount=m_tries;
			m_statusCode=ExitCode::BASE_IMAGE_MISSING;
		}
	};

	setCtrlCallback();
}

//--------------------------------------------------------------------

CtrlCommand::~CtrlCommand()
{
	std::string sampleImg="sample_"+m_baseImageName;
	removeImage(sampleImg);
	if(m_cleanImg){
		removeImg();
	}
}

//--------------------------------------------------------------------

void CtrlCommand::setCtrlCallback()
{
	std::string sampleImg="sample_"+m_baseImageName;

	std::string screenshotCmd;

	if(m_roiStr.length()>0){
		screenshotCmd=mkScreenshotStrCmd(m_windowName, sampleImg, m_roiStr);
	}
	else{
		screenshotCmd=mkScreenshotStrCmd(m_windowName, sampleImg);
	}
	
	std::string smpImgPath=getImgPath(sampleImg);

	bool baseImageExists=imageExists(m_baseImageName);

	m_cbk=[this, screenshotCmd, smpImgPath, baseImageExists](){
		
		if(baseImageExists){
			m_statusCode=ExitCode::TARGET_WINDOW_CLOSED;
			if(windowExists()){
				m_statusCode=ExitCode::SYSTEM_FAILED;
				if(0==system(screenshotCmd.c_str())){
					m_statusCode=ExitCode::OK;
					try{
						return GetImageDifference()->isSimilar(smpImgPath.c_str(), m_threshold, m_sensitivity);
					}
					catch(const std::exception& e){
						m_statusCode=ExitCode::CV_EXCEPTION;
					}
				}
			}
		}
		return !m_similarity;
	};
}

//--------------------------------------------------------------------

void CtrlCommand::updateBaseImg(const char* baseImg, const char* roiStr)
{
	//removeImg(); do not remove the image because we do not know if it is used by another test
	m_baseImageName=baseImg;
	m_roiStr=roiStr;
	setCtrlCallback();	
}

//--------------------------------------------------------------------

bool CtrlCommand::ready()
{
	dbg("ctrl cmd ready");

	bool result=m_cbk();
	if(!m_similarity){
		result=!result;
	}

	if(m_statusCode==ExitCode::OK && !result){
		m_statusCode=ExitCode::FAILED;
	}
	
	if(m_tries<++m_triesCount){
		if(!result && m_statusCode<2){
			m_statusCode=ExitCode::TIMEOUT;
		}
		result=true;// we should return true even if it timeout, because true will break the loop
	}

	return result;
}

//--------------------------------------------------------------------

void CtrlCommand::removeImg()
{
	// remove sample image too
	if(removeImage(m_baseImageName)){
		m_baseImageName="";
	}
}

//====================================================================
