/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
*         	                                                         *
* template<CommandTypes CT> struct CmdType2Bdr                       *
* template<CommandTypes CMDT> struct CmdBuilder                      *
* BaseCommand* ParserBuilder(const std::string& line);               *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "input_command.h"

//====================================================================

template<CommandTypes CT>
struct CmdType2Bdr
{
	//typedef MouseSelectCommand Cmd;
};

/*
template<>
struct CmdType2Bdr<CommandTypes::Screenshot>
{
};//*/

template<>
struct CmdType2Bdr<CommandTypes::Ctrl>
{
	typedef CtrlCommand Cmd;
};

template<>
struct CmdType2Bdr<CommandTypes::Keyboard>
{
	typedef KeyCommad Cmd;
};

template<>
struct CmdType2Bdr<CommandTypes::KeyboardLine>
{
	typedef LineCommand Cmd;
};

template<>
struct CmdType2Bdr<CommandTypes::KeyboardText>
{
	typedef TextCommand Cmd;
};

template<>
struct CmdType2Bdr<CommandTypes::Unicode>
{
	typedef UnicodeCommand Cmd;
};

template<>
struct CmdType2Bdr<CommandTypes::Shortcut>
{
	typedef ShortcutCommand Cmd;
};

template<>
struct CmdType2Bdr<CommandTypes::MouseMove>
{
	typedef MoveMouseCommand Cmd;
};

template<>
struct CmdType2Bdr<CommandTypes::MouseBtn>
{
	typedef MouseBtnCommand Cmd;
};

template<>
struct CmdType2Bdr<CommandTypes::MouseSelection>
{
	typedef MouseSelectCommand Cmd;
};


template<>
struct CmdType2Bdr<CommandTypes::MouseDrag>
{
	typedef MouseDragCommand Cmd;
};

template<>
struct CmdType2Bdr<CommandTypes::DoubleClick>
{
	typedef DoubleClickCommand Cmd;
};

//====================================================================

class CmdScrolledWindow;

void ParserBuilder(CmdScrolledWindow* cmdscrolledWindowPtr, bool indentation, const std::string& commandStr);

//====================================================================

#endif
