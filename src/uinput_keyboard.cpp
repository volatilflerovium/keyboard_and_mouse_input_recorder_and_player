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
#include "key_map.h"
#include "debug_utils.h"

#include <thread>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>

#define KEYBOARD_NAME "AutomaticTester keyboard"

//====================================================================

UinputKeyboard::UinputKeyboard()
: m_fd(open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_CLOEXEC))
{
	m_shortcutParserKeyMapPtr=&shortcutParserKeyMap;

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

//--------------------------------------------------------------------

UinputKeyboard::~UinputKeyboard()
{
	ioctl(m_fd, UI_DEV_DESTROY);
	close(m_fd);
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

void UinputKeyboard::sendKey(int hidCode1, int hidCode2, int hidCode3, int hidCode4, int hidCode5, int hidCode6)
{
	int max=0;
	int codes[]={hidCode1, hidCode2, hidCode3, hidCode4, hidCode5, hidCode6};
   for(int i=0; i<MAX_HID_CODES; i++){
		if(codes[i]<0){
			break;
		}
		max=i+1;
		emit(EV_KEY, codes[i], 1);		
	}
	emit(EV_SYN, SYN_REPORT, 0);

	for(int i=0; i<max; i++){
		emit(EV_KEY, codes[i], 0);
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
	addCombo('\n', KEY_ENTER);
	addCombo(' ', KEY_SPACE);
	addCombo('	', KEY_TAB);
	addCombo('\t', KEY_TAB);
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
