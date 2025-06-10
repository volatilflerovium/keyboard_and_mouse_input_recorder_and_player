/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* TinyUSBKeyboard class                                              *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "tinyusb_keyboard.h"

#include "debug_utils.h"

//--------------------------------------------------------------------

TinyUSBKeyboard::TinyUSBKeyboard()
{
	setLastError([this](){
		return !s_connector->isActive();
	});

	m_keyMap=&tinyusbKeyMap;
}

//--------------------------------------------------------------------

void TinyUSBKeyboard::sendKey(const KeyCombo& keyCodes)
{
	uint8_t data[MAX_HID_CODES+2]={0xE8, 0xE9, 0xE9, 0xE9, 0xE9, 0xE9, 0xE9, 0xE9};

	int dataSize=0;
	for(int i=0; i<MAX_HID_CODES; i++){
		if(keyCodes[i]<0){
			dataSize=i;
			break;
		}
		data[1+i]=static_cast<uint8_t>(keyCodes[i]);
	}

	if(dataSize>0){
		dataSize+=2;
		sendData(data, dataSize);
	}
}

//--------------------------------------------------------------------

bool TinyUSBKeyboard::isActive()
{
	return s_connector->isActive();
}

//--------------------------------------------------------------------

void TinyUSBKeyboard::addWhiteCharacters()
{
	addCombo(UTF8Char(u8"\n"), HID_KEY_ENTER);
	addCombo(UTF8Char(u8" "), HID_KEY_SPACE);
	addCombo(UTF8Char(u8"	"), HID_KEY_TAB);
	addCombo(UTF8Char(u8"\t"), HID_KEY_TAB);
}

//--------------------------------------------------------------------

void TinyUSBKeyboard::numLk()
{
	sendKey(HID_KEY_NUM_LOCK);
}

//--------------------------------------------------------------------

void TinyUSBKeyboard::prepareUnicodeInput()
{
	sendKey(HID_KEY_CONTROL_RIGHT, HID_KEY_SHIFT_LEFT, HID_KEY_U);
}

//--------------------------------------------------------------------
