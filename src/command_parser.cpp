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
#include "utilities.h"
#include "cstr_split.h"

//====================================================================

namespace CTRL_INDEX
{
	enum
	{
		ID=0,
		DESCRIPTION,
		RUN,
		BASE_IMAGE,
		ROI_STR,
		WINDOW_NAME,
		SIMILARITY,
		THRESHOLD,
		SENSITIVITY,
		STRICT_RUN,
		TIMEOUT,
	};
};

//====================================================================

BaseCommand* ParserBuilder(const std::string& line)
{
	CstrSplit<20> parts(line.c_str(), SEPARATOR);
	const int last=parts.dataSize()-1;

	auto toBool=[](const char* b){
		return memcmp("true", b, 4)==0;
	};

	BaseCommand* commandPtr=nullptr;	
	CommandTypes commandID=static_cast<CommandTypes>(std::atoi(parts[0]));

	const char* description=parts[1];
	bool run=toBool(parts[2]);
	const int wait=std::atoi(parts[last]);

	if(CommandTypes::Ctrl==commandID || CommandTypes::Screenshot==commandID){
		bool similarity=toBool(parts[CTRL_INDEX::SIMILARITY]);

		CtrlCommand* tmpPtr=nullptr;
		if(CommandTypes::Ctrl==commandID){
			tmpPtr=new CtrlCommand(description, parts[CTRL_INDEX::BASE_IMAGE], parts[CTRL_INDEX::ROI_STR], parts[CTRL_INDEX::WINDOW_NAME], false);
		}

		tmpPtr->setSimilarity(similarity);
		tmpPtr->updateActive(run);
		tmpPtr->setThreshold(std::atoi(parts[CTRL_INDEX::THRESHOLD]));
		tmpPtr->setSensitivity(std::atoi(parts[CTRL_INDEX::SENSITIVITY]));
		tmpPtr->setRestriction(toBool(parts[CTRL_INDEX::STRICT_RUN]));
		tmpPtr->updateTime(std::atoi(parts[CTRL_INDEX::TIMEOUT]));
		commandPtr=tmpPtr;
	}
	else{
		/*Input command
		 command ID ,  m_description, m_run, (params...), CMD_ID, m_wait
		*/
		switch(commandID)
		{
			case CommandTypes::Keyboard:
				commandPtr=CmdBuilder<CommandTypes::Keyboard>::Builder(run, description, wait, std::atoi(parts[3]));
				break;
			case CommandTypes::KeyboardLine:
				commandPtr=CmdBuilder<CommandTypes::KeyboardLine>::Builder(run, description, wait, std::string(parts[3], parts.chunkSize(3)));
				break;
			case CommandTypes::KeyboardText:
				commandPtr=CmdBuilder<CommandTypes::KeyboardText>::Builder(run, description, wait, std::string(parts[3], parts.chunkSize(3)));
				break;
			case CommandTypes::MouseMove:
				commandPtr=CmdBuilder<CommandTypes::MouseMove>::Builder(run, description, wait, std::atoi(parts[3]), std::atoi(parts[4]), parts[5]);
				break;
			case CommandTypes::MouseLeftBtn:
				commandPtr=CmdBuilder<CommandTypes::MouseLeftBtn>::Builder(run, description, wait, std::atoi(parts[3]), std::atoi(parts[4]), parts[5]);
				break;
			case CommandTypes::MouseRightBtn:
				commandPtr=CmdBuilder<CommandTypes::MouseRightBtn>::Builder(run, description, wait, std::atoi(parts[3]), std::atoi(parts[4]), parts[5]);
				break;
			case CommandTypes::MouseSelection:
				commandPtr=CmdBuilder<CommandTypes::MouseSelection>::Builder(run, description, wait, std::atoi(parts[3]), std::atoi(parts[4]), std::atoi(parts[5]), std::atoi(parts[6]), parts[7]);
				break;
			case CommandTypes::MouseDrag:
				{
					if(std::atoi(parts[3])<0){
						commandPtr=CmdBuilder<CommandTypes::MouseDrag>::Builder(run, description, wait, std::atoi(parts[5]), std::atoi(parts[6]), parts[7]);
					}
					else{
						commandPtr=CmdBuilder<CommandTypes::MouseDrag>::Builder(run, description, wait, std::atoi(parts[3]), std::atoi(parts[4]), std::atoi(parts[5]), std::atoi(parts[6]), parts[7]);
					}
				}
				break;
			case CommandTypes::Shortcut:
				commandPtr=CmdBuilder<CommandTypes::Shortcut>::Builder(run, description, wait, parts[3]);
				break;
			case CommandTypes::Unicode:
				commandPtr=CmdBuilder<CommandTypes::Unicode>::Builder(run, description, wait, parts[3]);
				break;
			default:
				dbg("Command builder not found");
				break;
		};
	}

	return commandPtr;
}

//====================================================================
