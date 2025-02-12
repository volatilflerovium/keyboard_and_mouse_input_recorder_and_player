/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class HIDManager                                                   *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "hid_manager.h"

#include "uinput_keyboard.h"
#include "uinput_mouse.h"

#include "tinyusb_connector.h"
#include "tinyusb_keyboard.h"
#include "tinyusb_mouse.h"

//====================================================================

ConnectorI* TinyusbConnector::s_connector=NullConnector::getConnector();

KeyboardEmulatorI* s_KeyboardEmulator=nullptr;

MouseEmulatorI* s_MouseEmulator=nullptr;

HID_TARGET HIDManager::s_currentTarget=HID_TARGET::NONE;

//====================================================================

void HIDManager::SetDummyEmulator()
{
	s_currentTarget=HID_TARGET::NONE;
	static DummyKeyboard keyboard;
	s_KeyboardEmulator=&keyboard;

	static DummyMouse mouse;
	s_MouseEmulator=&mouse;
}

//--------------------------------------------------------------------

void HIDManager::SetUinputEmulator()
{
	s_currentTarget=HID_TARGET::UINPUT;
	static UinputKeyboard keyboard;
	static bool init=false;
	if(!init){
		init=true;
		keyboard.loadPrintableCharacters();
	}

	s_KeyboardEmulator=&keyboard;

	static UinputMouse mouse;
	s_MouseEmulator=&mouse;
}

//--------------------------------------------------------------------

void HIDManager::SetTinyusbEmulator(const char* alpha, uint numeric, bool isSerial)
{
	s_currentTarget=HID_TARGET::TINYUSB;

	TinyusbConnector::setConnector(isSerial, alpha, numeric);

	dbg("isActive: ", TinyusbConnector::s_connector->isActive());

	static TinyUSBKeyboard keyboard;
	static bool init=false;
	if(!init){
		init=true;
		keyboard.loadPrintableCharacters();
	}

	s_KeyboardEmulator=&keyboard;

	static TinyusbMouse mouse;
	s_MouseEmulator=&mouse;
}

//====================================================================
