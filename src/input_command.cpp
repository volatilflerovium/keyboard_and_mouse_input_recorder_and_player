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
#include "ImageDiff_Lib/simple_image_difference.h"
#include "keyboard_emulator.h"
#include "mouse_emulator.h"
#include "settings_manager.h"

//====================================================================

extern MouseEmulatorI* s_MouseEmulator;
extern KeyboardEmulatorI* s_KeyboardEmulator;

int MouseCmdExitPosition::s_x=0;
int MouseCmdExitPosition::s_y=0;

void MouseCmdExitPosition::setExitPosition()
{
	s_x=wxGetMousePosition().x;
	s_y=wxGetMousePosition().y;
}

//====================================================================

const char* const ExitCode::ExitCodeVerbose[]=
{
	"OK",                          //=0,
	"Failed",                      //FAILED
	"Timeout",                     //TIMEOUT,
	"Control image missing",       //BASE_IMAGE_MISSING,
	"Target window is closed",     //TARGET_WINDOW_CLOSED,
	"Dimensions of control image and sample image\ndo not match. Possible cause: window resized or image corruption",            //CV_EXCEPTION
	"The input position for the command\nwas out of the visible screen",//OUT_OF_BOUND, when pointer is trying to get to a position outside of the screen
	"System error",                 //SYSTEM_FAILED,
	"Missing symbol",
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
	s_MouseEmulator->go2Position(x, y);

	s_MouseEmulator->clickBtn(MOUSE_BTN::LEFT);
}

//====================================================================

int ExitCode::ln(int x)
{
	if(x<ExitCode::LAST){
		int n=0;
		while(x>1){
			x=x>>1;
			n++;
		}
		if(n>0){
			n++;
		}
		return n;
	}
	return ln(ExitCode::UNKNOWN);
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

TextCommand::TextCommand(const char8_t* description, int wait, const std::u8string& text)
:InputCommand(description, wait)
, m_text(text)
{
	m_cmd=[this](){
		m_statusCode=ExitCode::OK;
		s_KeyboardEmulator->inputText(m_text.data());
		if(s_KeyboardEmulator->getErrorCode()>0){
			m_statusCode=ExitCode::MISSING_SYMBOL;
		}
	};
}

//--------------------------------------------------------------------

void TextCommand::print(std::ostream& outputStream)
{
	int ID=static_cast<int>(CommandTypes::KeyboardText);
	SimpleSerialization json;
	
	json.ToString(
		"ID", ID,
		"description", reinterpret_cast<const char*>(m_description.data()),
		"run", m_run,
		"text", reinterpret_cast<const char*>(m_text.data()),
		"wait", m_wait
		);
	json.dump(outputStream);
}

//====================================================================

LineCommand::LineCommand(const char8_t* description, int wait, const std::u8string& line)
:InputCommand(description, wait)
, m_line(line)
{
	m_cmd=[this](){
		m_statusCode=ExitCode::OK;
		s_KeyboardEmulator->inputLine(m_line.data());
		if(s_KeyboardEmulator->getErrorCode()>0){
			m_statusCode=ExitCode::MISSING_SYMBOL;
		}
	};
}

//--------------------------------------------------------------------

void LineCommand::print(std::ostream& outputStream)
{
	int ID=static_cast<int>(CommandTypes::KeyboardLine);

	SimpleSerialization json;
	
	json.ToString(
		"ID", ID,
		"description", reinterpret_cast<const char*>(m_description.data()),
		"run", m_run,
		"line", reinterpret_cast<const char*>(m_line.data()),
		"wait", m_wait
		);

	json.dump(outputStream);
}

//====================================================================

KeyCommad::KeyCommad(const char8_t* description, int wait, SPKEYS keyCode)
:InputCommand(description, wait)
, m_keyCode(keyCode)
{
	m_cmd=[this](){
		s_KeyboardEmulator->commandKey(m_keyCode);
	};
}

//--------------------------------------------------------------------

void KeyCommad::print(std::ostream& outputStream)
{
	int ID=static_cast<int>(CommandTypes::Keyboard);

	SimpleSerialization json;
	
	json.ToString(
		"ID", ID,
		"description", reinterpret_cast<const char*>(m_description.data()),
		"run", m_run,
		"keycode", int(m_keyCode),
		"wait", m_wait
		);

	json.dump(outputStream);
}

//====================================================================

UnicodeCommand::UnicodeCommand(const char8_t* description, int wait, const char8_t* codePoint)
:InputCommand(description, wait)
, m_codePoint(codePoint)
{
	m_cmd=[this](){
		s_KeyboardEmulator->unicodeCharacter(m_codePoint.data());
	};
}

//--------------------------------------------------------------------

void UnicodeCommand::print(std::ostream& outputStream)
{
	int ID=static_cast<int>(CommandTypes::Unicode);

	SimpleSerialization json;
	
	json.ToString(
		"ID", ID,
		"description", reinterpret_cast<const char*>(m_description.data()),
		"run", m_run,
		"codePoint", reinterpret_cast<const char*>(m_codePoint.data()),
		"wait", m_wait
		);

	json.dump(outputStream);
}

//====================================================================

ShortcutCommand::ShortcutCommand(const char8_t* description, int wait, const KeyCombo& shortcutCombo)
:InputCommand(description, wait)
, m_shortcutCombo(shortcutCombo)
{
	m_cmd=[this](){
		s_KeyboardEmulator->shortcut(m_shortcutCombo);
	};
}

//--------------------------------------------------------------------

void ShortcutCommand::print(std::ostream& outputStream)
{
	int ID=static_cast<int>(CommandTypes::Shortcut);

	std::string shortcutStr;
	for(int i=0; i<MAX_HID_CODES; i++){
		if(i>0){
			shortcutStr.append(":");
		}
		shortcutStr.append(std::to_string(m_shortcutCombo[i]));	
	}

	SimpleSerialization json;
	
	json.ToString(
		"ID", ID,
		"description", reinterpret_cast<const char*>(m_description.data()),
		"run", m_run,
		"shortcut", shortcutStr,
		"wait", m_wait
		);
	json.dump(outputStream);
}

//--------------------------------------------------------------------

ShortcutCommand* ShortcutCommand::Builder(const char8_t* description, int wait, const KeyCombo& shortcutCombo)
{
	return new ShortcutCommand(description, wait, shortcutCombo);
}

//====================================================================

MoveMouseCommand::MoveMouseCommand(const char8_t* description, int wait, int x, int y, const char* windowName)
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
				s_MouseEmulator->go2Position(m_absoluteX, m_absoluteY);
			}
		}
	};
}

//--------------------------------------------------------------------

void MoveMouseCommand::print(std::ostream& outputStream)
{
	int ID=static_cast<int>(CommandTypes::MouseMove);

	SimpleSerialization json;
	
	json.ToString(
		"ID", ID,
		"description", reinterpret_cast<const char*>(m_description.data()),
		"run", m_run,
		"x", m_x,
		"y", m_y,
		"windowName", m_windowName,
		"wait", m_wait
		);

	json.dump(outputStream);
}

//====================================================================

MouseBtnCommand::MouseBtnCommand(const char8_t* description, int wait, int x, int y, MOUSE_BTN btn, const char* windowName)
:InputCommand(description, wait)
, WindowOffset(windowName)
, m_x(x)
, m_y(y)
, m_pressForMs(0)
, m_btn(btn)
{
	m_cmd=[this](){
		m_statusCode=ExitCode::TARGET_WINDOW_CLOSED;
		if(windowExists()){
			m_statusCode=ExitCode::OUT_OF_BOUND;
			if(isTargetValid(m_x, m_y)){
				m_statusCode=ExitCode::OK;
				s_MouseEmulator->go2Position(m_absoluteX, m_absoluteY);
				s_MouseEmulator->clickBtn(m_btn, m_pressForMs);
			}
		}
		MouseCmdExitPosition::setExitPosition();
	};
}

//--------------------------------------------------------------------

void MouseBtnCommand::print(std::ostream& outputStream)
{
	int ID=static_cast<int>(CommandTypes::MouseBtn);		
	SimpleSerialization json;
	
	json.ToString(
		"ID", ID,
		"description", reinterpret_cast<const char*>(m_description.data()),
		"run", m_run,
		"x", m_x,
		"y", m_y,
		"btn", static_cast<int>(m_btn),
		"pressFor", m_pressForMs, 
		"windowName", m_windowName,
		"wait", m_wait
		);
	json.dump(outputStream);
}

//====================================================================

DoubleClickCommand::DoubleClickCommand(const char8_t* description, int wait, int x, int y, MOUSE_BTN btn, const char* windowName)
:InputCommand(description, wait)
, WindowOffset(windowName)
, m_x(x)
, m_y(y)
, m_btn(btn)
{
	m_cmd=[this](){
		m_statusCode=ExitCode::TARGET_WINDOW_CLOSED;
		if(windowExists()){
			m_statusCode=ExitCode::OUT_OF_BOUND;
			if(isTargetValid(m_x, m_y)){
				m_statusCode=ExitCode::OK;
				s_MouseEmulator->go2Position(m_absoluteX, m_absoluteY);
				s_MouseEmulator->doubleClickBtn(m_btn, SettingsManager::getSettingManager().getDoubleClick());
			}
		}
		//MouseCmdExitPosition::setExitPosition();
	};
}

//--------------------------------------------------------------------

void DoubleClickCommand::print(std::ostream& outputStream)
{
	int ID=static_cast<int>(CommandTypes::DoubleClick);		
	SimpleSerialization json;
	
	json.ToString(
		"ID", ID,
		"description", reinterpret_cast<const char*>(m_description.data()),
		"run", m_run,
		"x", m_x,
		"y", m_y,
		"btn", static_cast<int>(m_btn),
		"windowName", m_windowName,
		"wait", m_wait
		);
	json.dump(outputStream);
}

//====================================================================

MouseSelectCommand::MouseSelectCommand(const char8_t* description, int wait, uint posX, uint posY, int width, int height, const char* windowName)
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
				s_MouseEmulator->select(m_absoluteX, m_absoluteY, m_width, m_height);
			}
		}
		MouseCmdExitPosition::setExitPosition();
	};

}

//--------------------------------------------------------------------

void MouseSelectCommand::print(std::ostream& outputStream)
{
	int ID=static_cast<int>(CommandTypes::MouseSelection);
	SimpleSerialization json;

	json.ToString(
		"ID", ID,
		"description", reinterpret_cast<const char*>(m_description.data()),
		"run", m_run,
		"posX", m_posX,
		"posY", m_posY,
		"width", m_width,
		"height", m_height,
		"windowName", m_windowName,
		"wait", m_wait
		);
	json.dump(outputStream);
}

//====================================================================

MouseDragCommand::MouseDragCommand(const char8_t* description, int wait, int startX, int startY, int endX, int endY, const char* windowName)
:InputCommand(description, wait)
, WindowOffset(windowName)
, m_startX(startX)
, m_startY(startY)
, m_endX(endX)
, m_endY(endY)
{
	m_cmd=[this](){
		m_statusCode=ExitCode::TARGET_WINDOW_CLOSED;
		if(windowExists()){
			m_statusCode=ExitCode::OUT_OF_BOUND;
			if(isTargetValid(m_startX, m_startY)){
				int absStartX=m_absoluteX;
				int absStartY=m_absoluteY;
				if(isTargetValid(m_endX, m_endY)){
					m_statusCode=ExitCode::OK;
					int absEndX=m_absoluteX;
					int absEndY=m_absoluteY;

					s_MouseEmulator->drag(absStartX, absStartY, absEndX, absEndY);
				}
			}
		}
		MouseCmdExitPosition::setExitPosition();
	};
}

//--------------------------------------------------------------------

MouseDragCommand::MouseDragCommand(const char8_t* description, int wait, int endX, int endY, const char* windowName)
:InputCommand(description, wait)
, WindowOffset(windowName)
, m_startX(-1)
, m_startY(-1)
, m_endX(endX)
, m_endY(endY)
{
	/*
	 * Notice that if the current mouse position is not the same as the
	 * position of it when the last mouse command exit (it was moved accidently)
	 * we might get different result. Therefore it would be better to keep
	 * tract of the mouse position according to the last mouse command executed.
	 * */
	m_cmd=[this](){
		m_statusCode=ExitCode::TARGET_WINDOW_CLOSED;
		if(windowExists()){
			m_statusCode=ExitCode::OUT_OF_BOUND;
			if(isTargetValid(m_endX, m_endY)){
				m_statusCode=ExitCode::OK;
				int startX=MouseCmdExitPosition::s_x;
				int startY=MouseCmdExitPosition::s_y;

				s_MouseEmulator->drag(startX, startY, m_absoluteX, m_absoluteY);
			}
		}
		MouseCmdExitPosition::setExitPosition();
	};

}

//--------------------------------------------------------------------

void MouseDragCommand::print(std::ostream& outputStream)
{
	int ID=static_cast<int>(CommandTypes::MouseDrag);
	SimpleSerialization json;
	
	json.ToString(
		"ID", ID,
		"description", reinterpret_cast<const char*>(m_description.data()),
		"run", m_run,
		"startX", m_startX,
		"startY", m_startY,
		"endX", m_endX,
		"endY", m_endY,
		"wait", m_wait,
		"windowName", m_windowName,
		"wait", m_wait
		);
	json.dump(outputStream);
}

//====================================================================

CtrlCommand::CtrlCommand(const char8_t* description, const std::string& baseImageName, const char* roiStr, const char* windowName, bool removeImg)
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

void CtrlCommand::updateBaseImg(const char* newBaseImg, const char* newRoiStr)
{
	//removeImg(); do not remove the image because we do not know if it is used by another test
	m_baseImageName=newBaseImg;
	m_roiStr=newRoiStr;
	setCtrlCallback();	
}

//--------------------------------------------------------------------

void CtrlCommand::updateBaseImg(const char* newBaseImg)
{
	//removeImg(); do not remove the image because we do not know if it is used by another test
	m_baseImageName=newBaseImg;
	setCtrlCallback();	
}

//--------------------------------------------------------------------

bool CtrlCommand::ready()
{
	bool result=m_cbk();
	if(!m_similarity){
		result=!result;
	}

	if(m_statusCode==ExitCode::OK && !result){
		m_statusCode=ExitCode::FAILED;
	}
	
	if(m_tries<++m_triesCount){
		result=true;// we should return true even if it timeout, because true will break the loop
	}

	return result;
}

//--------------------------------------------------------------------

void CtrlCommand::print(std::ostream& outputStream)
{
	m_cleanImg=false;

	SimpleSerialization json;
	
	json.ToString(
		"ID", static_cast<int>(CommandTypes::Ctrl),
		"description", reinterpret_cast<const char*>(m_description.data()),
		"run", m_run,
		"baseImageName", m_baseImageName,
		"roiStr", m_roiStr,
		"windowName", m_windowName,
		"similarity", m_similarity,
		"threshold", m_threshold,
		"sensitivity", m_sensitivity,
		"strictRun", m_strictRun,
		"timeout", getTimeout() // wait
		);

	json.dump(outputStream);
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
