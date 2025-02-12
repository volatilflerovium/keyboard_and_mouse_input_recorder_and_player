/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* enum class SPKEYS                                                  *
* class KeyConversion                                                *
* class ComboStringParser                                            *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "key_conversion.h"
#include "tinyusb_key_map.h"

#include <cstring> 
#include <linux/uinput.h>

//====================================================================

uint KeyConversion::getKeyCode(SPKEYS keyCode, ConvCode _getKeyCode)
{
	if(keyCode<SPKEYS::INSERT){
		if(keyCode<SPKEYS::F7){
			switch(keyCode){
				case SPKEYS::NONE:
					return 0;
				break;
				case SPKEYS::ESC:
					return _getKeyCode(KEY_ESC, HID_KEY_ESCAPE);
				break;
				case SPKEYS::SCROLLLOCK:
					return _getKeyCode(KEY_SCROLLLOCK, HID_KEY_SCROLL_LOCK);
				break;
				case SPKEYS::PAUSE:
					return _getKeyCode(KEY_PAUSE, HID_KEY_PAUSE);
				break;
				case SPKEYS::F1:
					return _getKeyCode(KEY_F1, HID_KEY_F1);
				break;
				case SPKEYS::F2:
					return _getKeyCode(KEY_F2, HID_KEY_F2);
				break;
				case SPKEYS::F3:
					return _getKeyCode(KEY_F3, HID_KEY_F3);
				break;
				case SPKEYS::F4:
					return _getKeyCode(KEY_F4, HID_KEY_F4);
				break;
				case SPKEYS::F5:
					return _getKeyCode(KEY_F5, HID_KEY_F5);
				break;
				case SPKEYS::F6:
					return _getKeyCode(KEY_F6, HID_KEY_F6);
				break;
				default:
					return 0;
				break;
			}
		}
		else{
			switch(keyCode){
				case SPKEYS::F7:
					return _getKeyCode(KEY_F7, HID_KEY_F7);
				break;
				case SPKEYS::F8:
					return _getKeyCode(KEY_F8, HID_KEY_F8);
				break;
				case SPKEYS::F9:
					return _getKeyCode(KEY_F9, HID_KEY_F9);
				break;
				case SPKEYS::F10:
					return _getKeyCode(KEY_F10, HID_KEY_F10);
				break;
				case SPKEYS::F11:
					return _getKeyCode(KEY_F11, HID_KEY_F11);
				break;
				case SPKEYS::F12:
					return _getKeyCode(KEY_F12, HID_KEY_F12);
				break;
				case SPKEYS::SYSRQ:
					return _getKeyCode(KEY_SYSRQ, HID_KEY_SYSREQ_ATTENTION); //<---- check
				break;
				case SPKEYS::BACKSPACE:
					return _getKeyCode(KEY_BACKSPACE, HID_KEY_BACKSPACE);
				break;
				case SPKEYS::ENTER:
					return _getKeyCode(KEY_ENTER, HID_KEY_ENTER);
				break;
				default:
					return 0;
				break;
			}
		}
	}
	else{
		if(keyCode<SPKEYS::RIGHT){
			switch(keyCode){
				case SPKEYS::INSERT:
					return _getKeyCode(KEY_INSERT, HID_KEY_INSERT);
				break;
				case SPKEYS::HOME:
					return _getKeyCode(KEY_HOME, HID_KEY_HOME);
				break;
				case SPKEYS::PAGEUP:
					return _getKeyCode(KEY_PAGEUP, HID_KEY_PAGE_UP);
				break;
				case SPKEYS::DELETE:
					return _getKeyCode(KEY_DELETE, HID_KEY_DELETE);
				break;
				case SPKEYS::END:
					return _getKeyCode(KEY_END, HID_KEY_END);
				break;
				case SPKEYS::PAGEDOWN:
					return _getKeyCode(KEY_PAGEDOWN, HID_KEY_PAGE_DOWN);
				break;
				case SPKEYS::UP:
					return _getKeyCode(KEY_UP, HID_KEY_ARROW_UP);
				break;
				case SPKEYS::LEFT:
					return _getKeyCode(KEY_LEFT, HID_KEY_ARROW_LEFT);
				break;
				case SPKEYS::DOWN:
					return _getKeyCode(KEY_DOWN, HID_KEY_ARROW_DOWN);
				break;
				default:
					return 0;
				break;
			}
		}
		else{
			switch(keyCode){
				case SPKEYS::RIGHT:
					return _getKeyCode(KEY_RIGHT, HID_KEY_ARROW_RIGHT);
				break;
				case SPKEYS::CAPSLOCK:
					return _getKeyCode(KEY_CAPSLOCK, HID_KEY_CAPS_LOCK);
				break;
				case SPKEYS::LEFTCTRL:
					return _getKeyCode(KEY_LEFTCTRL, HID_KEY_CONTROL_LEFT);
				break;
				case SPKEYS::RIGHTCTRL:
					return _getKeyCode(KEY_RIGHTCTRL, HID_KEY_CONTROL_RIGHT);
				break;
				case SPKEYS::LEFTALT:
					return _getKeyCode(KEY_LEFTALT, HID_KEY_ALT_LEFT);
				break;
				case SPKEYS::RIGHTALT:
					return _getKeyCode(KEY_RIGHTALT, HID_KEY_ALT_RIGHT);
				break;
				case SPKEYS::LEFTSHIFT:
					return _getKeyCode(KEY_LEFTSHIFT, HID_KEY_SHIFT_LEFT);
				break;
				case SPKEYS::RIGHTSHIFT:
					return _getKeyCode(KEY_RIGHTSHIFT, HID_KEY_SHIFT_RIGHT);
				break;
				case SPKEYS::LEFTMETA:
					return _getKeyCode(KEY_LEFTMETA, HID_KEY_GUI_LEFT);
				break;
				case SPKEYS::RIGHTMETA:
					return _getKeyCode(KEY_RIGHTMETA, HID_KEY_GUI_RIGHT);
				break;
				default:
					return 0;
				break;
			}
		}
	}
	return 0;
}

//====================================================================

ComboStringParser::ComboStringParser(const std::string& shortcutStr)
:m_str(nullptr)
, m_count(0)
{
	m_str=new char[1+shortcutStr.length()];
	std::memset(m_str, 0, sizeof(char)*(1+shortcutStr.length()));
	int pos=0;
	size_t pl=0;
	size_t pr=0;
	size_t pt=0;
	while(pl!=std::string::npos){
		pl=shortcutStr.find_first_not_of(" +", pr);// ' ' and '+'
		pr=shortcutStr.find(" ", pl);
		pt=shortcutStr.find("+", pl);
		if(pr==std::string::npos || pr>pt){
			pr=pt;
		}

		if(pl!=std::string::npos){	
			std::string tmp;
			size_t ql=0;
			size_t qr=(pr!=std::string::npos)? pr : shortcutStr.length();
			
			if(trim(shortcutStr, pl, ql, qr)){
				if(qr>pr){
					qr=pr;
				}
				tmp=shortcutStr.substr(ql, qr-ql);
				m_parts[m_count++]=m_str+pos;
				std::memcpy(m_str+pos, tmp.c_str(), tmp.length());
				pos+=tmp.length()+1;
			}
		}
	}
}

//--------------------------------------------------------------------

ComboStringParser::ComboStringParser(const ComboStringParser& other)
:m_count(other.m_count)
{
	uint length=0;
	for(uint i=0; i<m_count; i++){
		length+=std::strlen(other.m_parts[i])+1;
	}

	m_str=new char[length];
	char* ptr=m_str;
	std::memcpy(m_str, other.m_str, sizeof(char)*length);
	for(uint i=0; i<m_count; i++){
		m_parts[i]=ptr;
		ptr+=std::strlen(other.m_parts[i])+1;
	}
}

//--------------------------------------------------------------------

ComboStringParser& ComboStringParser::operator=(const ComboStringParser& other)
{
	m_count=other.m_count;
	if(m_str){
		delete[] m_str;
		m_str=nullptr;
	}

	if(m_count>0){
		uint length=0;
		for(uint i=0; i<m_count; i++){
			length+=std::strlen(other.m_parts[i])+1;
		}
		char* ptr=m_str;
		m_str=new char[length];
		std::memcpy(m_str, other.m_str, sizeof(char)*length);
		for(uint i=0; i<m_count; i++){
			m_parts[i]=ptr;
			ptr+=std::strlen(other.m_parts[i])+1;
		}
	}
	return *this;
}

//--------------------------------------------------------------------

ComboStringParser::ComboStringParser(ComboStringParser&& other)
:m_str(other.m_str)
, m_count(other.m_count)
{
	for(uint i=0; i<m_count; i++){
		m_parts[i]=other.m_parts[i];
	}
	other.m_str=nullptr;
	other.m_count=0;
}

//--------------------------------------------------------------------

bool ComboStringParser::trim(const std::string& str, size_t offset, size_t& pl, size_t& pr)
{
	pl=str.find_first_not_of(' ', offset);
	if(pl!=std::string::npos){
		size_t tmp=pr;
		pr=str.find_first_of(' ', pl);
		if(pr==std::string::npos){
			pr=tmp;
		}
	}
	return pl!=std::string::npos;
}

//--------------------------------------------------------------------

bool ComboStringParser::toKeycode(const std::map<std::string, int>* shortcutParserKeyMapPtr,
	int (&keyCodes)[MAX_HID_CODES]) const
{
	keyCodes[0]=-1;
	keyCodes[1]=-1;
	keyCodes[2]=-1;
	keyCodes[3]=-1;
	keyCodes[4]=-1;
	keyCodes[5]=-1;
	uint idx=0;
	std::map<std::string, int>::const_iterator it;
	for(uint i=0; i<m_count; i++){
		it=shortcutParserKeyMapPtr->find(getPart(i));
		if(it!=shortcutParserKeyMapPtr->end()){
			keyCodes[idx++]=it->second;
		}
	}
	return idx>0;
}

//====================================================================
