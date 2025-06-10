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
#ifndef _KEY_CONVERSION_H
#define _KEY_CONVERSION_H
#include "key_index.h"
#include "keycombo.h"

#include <string> // for uint
#include <map>
#include <functional>

//====================================================================

enum class SPKEYS
{
	NONE=0,
	ESC,
	SCROLLLOCK,
	PAUSE,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	PRINT_SCREEN,
	BACKSPACE,
	ENTER,
	INSERT,
	HOME,
	PAGEUP,
	DELETE,
	END,
	PAGEDOWN,
	UP,
	LEFT,
	DOWN,
	RIGHT,
	CAPSLOCK,
	LEFTCTRL,
	RIGHTCTRL,
	LEFTALT,
	RIGHTALT,
	LEFTSHIFT,
	RIGHTSHIFT,
	LEFTMETA,
	RIGHTMETA,
	TAB,
	NUMLOCK,
	_LAST,
};

//--------------------------------------------------------------------

class TinyUSBKeyboard;
class UinputKeyboard;

class KeyConversion
{
	typedef std::function<uint(uint, uint)> ConvCode;

	public:
		template<typename T>
		static uint getKeyCode(SPKEYS keyCode)
		{
			return 0;
		}

	private:
		static uint getKeyCode(SPKEYS keyCode, ConvCode _getKeyCode);
};

template<>
inline uint KeyConversion::getKeyCode<UinputKeyboard>(SPKEYS keyCode)
{
	return getKeyCode(keyCode, [](uint uinputKey, uint tinyusbKey){
		return uinputKey;
	});
}

template<>
inline uint KeyConversion::getKeyCode<TinyUSBKeyboard>(SPKEYS keyCode)
{
	return getKeyCode(keyCode, [](uint uinputKey, uint tinyusbKey){
		return tinyusbKey;
	});
}

//====================================================================

#endif
