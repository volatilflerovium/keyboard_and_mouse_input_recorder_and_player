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
	m_shortcutParserKeyMapPtr=&hidShortcutParserKeyMap;

	setLastError([this](){
		return !s_connector->isActive();
	});
}

//--------------------------------------------------------------------

void TinyUSBKeyboard::sendKey(int keyCode)
{
	uint8_t data[3]={0xE8, static_cast<uint8_t>(keyCode), 0xE9};
	sendData(data, 3);
}

//--------------------------------------------------------------------

void TinyUSBKeyboard::sendKey(int hidCode1, int hidCode2)
{
	uint8_t data[4]={0xE8, static_cast<uint8_t>(hidCode1), static_cast<uint8_t>(hidCode2), 0xE9};
	sendData(data, 4);
}

//--------------------------------------------------------------------

void TinyUSBKeyboard::sendKey(int hidCode1, int hidCode2, int hidCode3, int hidCode4, int hidCode5, int hidCode6)
{
	int rawData[]={hidCode1, hidCode2, hidCode3, hidCode4, hidCode5, hidCode6};

	uint8_t data[MAX_HID_CODES+2]={0xE8, 0xE9, 0xE9, 0xE9, 0xE9, 0xE9, 0xE9, 0xE9};

	int dataSize=0;
	for(int i=0; i<MAX_HID_CODES; i++){
		if(rawData[i]<0){
			break;
		}
		dataSize=i+1;
		data[1+i]=static_cast<uint8_t>(rawData[i]);
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
	addCombo('\n', HID_KEY_ENTER);
	addCombo(' ', HID_KEY_SPACE);
	addCombo('	', HID_KEY_TAB);
	addCombo('\t', HID_KEY_TAB);
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
