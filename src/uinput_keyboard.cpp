/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class UinputKeyboard                                               *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "uinput_keyboard.h"
#include "debug_utils.h"

#include <thread>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>

#define KEYBOARD_NAME "kmRecorderPlayer keyboard"

//====================================================================

UinputKeyboard::UinputKeyboard()
: m_fd(-1)
{
	reload();

	m_keyMap=&uinputKeyMap;
}

//--------------------------------------------------------------------

UinputKeyboard::~UinputKeyboard()
{
	ioctl(m_fd, UI_DEV_DESTROY);
	close(m_fd);
}

//--------------------------------------------------------------------

bool UinputKeyboard::reload()
{
	if(m_fd<0){
		m_fd=open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_CLOEXEC);

		setLastError([this](){
			return m_fd < 1;
		});

		if(m_fd>-1){
			ioctl(m_fd, UI_SET_EVBIT, EV_KEY);

			for(const auto& [key, value] : uinputKeyMap){
				ioctl(m_fd, UI_SET_KEYBIT, value);
			}
			init(KEYBOARD_NAME);
		}			
	}

	return m_fd>-1;
}

//--------------------------------------------------------------------

void UinputKeyboard::init(const char* deviceName)
{
	std::memset(&m_usetup, 0, sizeof(m_usetup));
	m_usetup.id.bustype = BUS_USB;
	m_usetup.id.vendor  = 0x1234;
	m_usetup.id.product = 0x5678;
	//m_usetup.id.version = 1;
	strcpy(m_usetup.name, deviceName);

	ioctl(m_fd, UI_DEV_SETUP, &m_usetup);
	ioctl(m_fd, UI_DEV_CREATE);
}

//--------------------------------------------------------------------

bool UinputKeyboard::emit(int type, int code, int val)
{
   m_inputEvent.type = type;
   m_inputEvent.code = code;
   m_inputEvent.value = val;
   // timestamp values below are ignored
   m_inputEvent.time.tv_sec = 0;
   m_inputEvent.time.tv_usec = 0;

   return 0<write(m_fd, &m_inputEvent, sizeof(m_inputEvent));
}

//--------------------------------------------------------------------

void UinputKeyboard::sendKey(int hidCode)
{
   emit(EV_KEY, hidCode, 1);
	emit(EV_SYN, SYN_REPORT, 0);
	emit(EV_KEY, hidCode, 0);
	emit(EV_SYN, SYN_REPORT, 0);
	std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

//--------------------------------------------------------------------

void UinputKeyboard::sendKey(int hidCode1, int hidCode2)
{
   emit(EV_KEY, hidCode1, 1);
   emit(EV_KEY, hidCode2, 1);
	emit(EV_SYN, SYN_REPORT, 0);
	emit(EV_KEY, hidCode1, 0);
	emit(EV_KEY, hidCode2, 0);
	emit(EV_SYN, SYN_REPORT, 0);
	std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

//--------------------------------------------------------------------

void UinputKeyboard::sendKey(int hidCode1, int hidCode2, int hidCode3)
{
   emit(EV_KEY, hidCode1, 1);
   emit(EV_KEY, hidCode2, 1);
   emit(EV_KEY, hidCode3, 1);
	emit(EV_SYN, SYN_REPORT, 0);
	emit(EV_KEY, hidCode1, 0);
	emit(EV_KEY, hidCode2, 0);
	emit(EV_KEY, hidCode3, 0);
	emit(EV_SYN, SYN_REPORT, 0);
	std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

//--------------------------------------------------------------------

void UinputKeyboard::sendKey(const KeyCombo& keyCodes)
{
	int max=0;
   for(int i=0; i<MAX_HID_CODES; i++){
		if(keyCodes[i]<0){
			max=i;
			break;
		}
		emit(EV_KEY, keyCodes[i], 1);		
	}
	emit(EV_SYN, SYN_REPORT, 0);

	for(int i=0; i<max; i++){
		emit(EV_KEY, keyCodes[i], 0);
	}
	emit(EV_SYN, SYN_REPORT, 0);

	std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

//--------------------------------------------------------------------

bool UinputKeyboard::isActive()
{
	return m_fd>-1;
}

//--------------------------------------------------------------------

void UinputKeyboard::addWhiteCharacters()
{
	addCombo(UTF8Char(u8"\n"), KEY_ENTER);
	addCombo(UTF8Char(u8" "), KEY_SPACE);
	addCombo(UTF8Char(u8"	"), KEY_TAB);
	addCombo(UTF8Char(u8"\t"), KEY_TAB);
}

//--------------------------------------------------------------------

void UinputKeyboard::prepareUnicodeInput()
{
	sendKey(KEY_RIGHTCTRL, KEY_LEFTSHIFT, KEY_U);
}

//--------------------------------------------------------------------

void UinputKeyboard::numLk()
{
}

//====================================================================
