/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* template<CommandTypes CT> struct CmdType2Bdr                       *
* template<CommandTypes CMDT> struct CmdBuilder                      *
* BaseCommand* ParserBuilder(const std::string& line);               *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "command_parser.h"
#include "cstr_split.h"
#include "cmd_scrolled_window.h"

//====================================================================

template<CommandTypes CMDT>
struct CmdBuilder
{
	typedef typename CmdType2Bdr<CMDT>::Cmd CMD;

	template<typename... Args>
	static CMD* Builder(bool run, Args... args)
	{
		CMD* ptr=CMD::Builder(args...);
		ptr->updateActive(run);
		return ptr;
	}
};

void ParserBuilder(CmdScrolledWindow* cmdscrolledWindowPtr, bool indentation, const std::string& commandStr)
{
	SimpleUnserialization<20> cmdData(commandStr.c_str(), SEPARATOR);

	CommandTypes commandID=static_cast<CommandTypes>(cmdData.get<int>("ID"));
	const char8_t* description=cmdData.get<const char8_t*>("description");
	bool run=cmdData.get<bool>("run");
	if(commandID==CommandTypes::Ctrl){
		CtrlCommand* cmdPtr=new CtrlCommand(
			description,
			cmdData.get<const char*>("baseImageName"),
			cmdData.get<const char*>("roiStr"),
			cmdData.get<const char*>("windowName"),
			false);

		cmdPtr->setSimilarity(cmdData.get<bool>("similarity"));
		cmdPtr->updateActive(run);
		cmdPtr->setThreshold(cmdData.get<int>("threshold"));
		cmdPtr->setSensitivity(cmdData.get<int>("sensitivity"));
		cmdPtr->setRestriction(cmdData.get<bool>("strictRun"));
		cmdPtr->updateTime(cmdData.get<int>("timeout"));

		cmdscrolledWindowPtr->addCommand(cmdPtr, indentation);
	}
	else{
		if(commandID==CommandTypes::Keyboard){
			auto cmdPtr=CmdBuilder<CommandTypes::Keyboard>::Builder(
				run, description,
				cmdData.get<int>("keycode"), cmdData.get<int>("wait"));
			cmdscrolledWindowPtr->addCommand(cmdPtr, indentation);
		}
		else if(commandID==CommandTypes::KeyboardLine){
			auto cmdPtr=CmdBuilder<CommandTypes::KeyboardLine>::Builder(
				run, description,
				cmdData.get<int>("wait"), cmdData.get<const char8_t*>("line"));
			cmdscrolledWindowPtr->addCommand(cmdPtr, indentation);
		}
		else if(commandID==CommandTypes::KeyboardText){
			auto cmdPtr=CmdBuilder<CommandTypes::KeyboardText>::Builder(
				run, description,
				cmdData.get<int>("wait"), cmdData.get<const char8_t*>("text"));
			cmdscrolledWindowPtr->addCommand(cmdPtr, indentation);
		}
		else if(commandID==CommandTypes::MouseBtn){
			auto cmdPtr=CmdBuilder<CommandTypes::MouseBtn>::Builder(
				run, description,
				cmdData.get<int>("wait"),
				cmdData.get<int>("x"), cmdData.get<int>("y"),
				static_cast<MOUSE_BTN>(cmdData.get<int>("btn")),
				cmdData.get<int>("pressFor"),
				cmdData.get<const char*>("windowName"));
			cmdscrolledWindowPtr->addCommand<MouseBtnCommand>(cmdPtr, indentation);
		}
		else if(commandID==CommandTypes::MouseMove){
			auto cmdPtr=CmdBuilder<CommandTypes::MouseMove>::Builder(
				run, description, cmdData.get<int>("wait"),
				cmdData.get<int>("x"), cmdData.get<int>("y"),
				cmdData.get<const char*>("windowName"));
			cmdscrolledWindowPtr->addCommand(cmdPtr, indentation);
		}
		else if(commandID==CommandTypes::MouseSelection){
			auto cmdPtr=CmdBuilder<CommandTypes::MouseSelection>::Builder(
				run, description, cmdData.get<int>("wait"),
				cmdData.get<int>("posX"), cmdData.get<int>("posY"),
				cmdData.get<int>("width"),
				cmdData.get<int>("height"),cmdData.get<const char*>("windowName"));
			cmdscrolledWindowPtr->addCommand(cmdPtr, indentation);
		}
		else if(commandID==CommandTypes::MouseDrag){
			auto cmdPtr=CmdBuilder<CommandTypes::MouseDrag>::Builder(
				run, description, cmdData.get<int>("wait"),
				cmdData.get<int>("startX"), cmdData.get<int>("startY"),
				cmdData.get<int>("endX"), cmdData.get<int>("endY"),
				cmdData.get<const char*>("windowName"));
			cmdscrolledWindowPtr->addCommand(cmdPtr, indentation);
		}
		else if(commandID==CommandTypes::Shortcut){
			CstrSplit<6> parts(cmdData.get<const char*>("shortcut"), ":");
			KeyCombo combo;
			for(int i=0; i<MAX_HID_CODES; i++){
				combo.pushBack(std::atoi(parts[i]));
				dbg(std::atoi(parts[i]));
			}	
			
			auto cmdPtr=CmdBuilder<CommandTypes::Shortcut>::Builder(
				run, description, cmdData.get<int>("wait"),
				combo);
			cmdscrolledWindowPtr->addCommand(cmdPtr, indentation);
		}
		else if(commandID==CommandTypes::Unicode){
			auto cmdPtr=CmdBuilder<CommandTypes::Unicode>::Builder(
				run, description, cmdData.get<int>("wait"),
				cmdData.get<const char8_t*>("codePoint"));
			cmdscrolledWindowPtr->addCommand(cmdPtr, indentation);
		}
		else if(commandID==CommandTypes::DoubleClick){
			auto cmdPtr=CmdBuilder<CommandTypes::DoubleClick>::Builder(
				run, description, cmdData.get<int>("wait"),
				cmdData.get<int>("x"), cmdData.get<int>("y"),
				static_cast<MOUSE_BTN>(cmdData.get<int>("btn")),
				cmdData.get<const char*>("windowName"));
			cmdscrolledWindowPtr->addCommand(cmdPtr, indentation);
		}
	}
}

//====================================================================
