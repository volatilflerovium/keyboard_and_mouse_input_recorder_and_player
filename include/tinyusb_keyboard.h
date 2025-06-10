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
#ifndef _TINYUSB_KEYBOARD_H
#define _TINYUSB_KEYBOARD_H

#include "keyboard_emulator.h"
#include "tinyusb_connector.h"
#include "tinyusb_keymap.h"
#include "key_conversion.h"

//====================================================================

class TinyUSBKeyboard : protected TinyusbConnector, public KeyboardEmulatorI
{
	public:
		TinyUSBKeyboard();

		virtual ~TinyUSBKeyboard()=default;

		virtual void numLk() override;
		virtual bool isActive() override;

		virtual void loadPrintableCharacters() override;

		virtual void commandKey(SPKEYS k1) override;

	private:
		virtual void sendKey(int keyCode) override;
		virtual void sendKey(int hidCode1, int hidCode2) override;
		virtual void sendKey(int hidCode1, int hidCode2, int hidCode3) override;
		virtual void sendKey(const KeyCombo& keyCodes) override;

		virtual void addWhiteCharacters() override;
		virtual void prepareUnicodeInput() override;

		void sendData(uint8_t* keyCodes, unsigned int N);
};

//--------------------------------------------------------------------

inline void TinyUSBKeyboard::loadPrintableCharacters()
{
	KeyboardEmulatorI::loadPrintableCharacters("tinyusb_printable_characters.txt");
}

//--------------------------------------------------------------------

inline void TinyUSBKeyboard::sendData(uint8_t* keyCodes, unsigned int N)
{
	sendAndWait(keyCodes, N);
}

//--------------------------------------------------------------------

inline void TinyUSBKeyboard::commandKey(SPKEYS k)
{
	sendKey(KeyConversion::getKeyCode<TinyUSBKeyboard>(k));
}

//--------------------------------------------------------------------

inline void TinyUSBKeyboard::sendKey(int keyCode)
{
	uint8_t data[3]={0xE8, static_cast<uint8_t>(keyCode), 0xE9};
	sendData(data, 3);
}

//--------------------------------------------------------------------

inline void TinyUSBKeyboard::sendKey(int hidCode1, int hidCode2)
{
	uint8_t data[4]={0xE8, static_cast<uint8_t>(hidCode1), static_cast<uint8_t>(hidCode2), 0xE9};
	sendData(data, 4);
}

//--------------------------------------------------------------------

inline void TinyUSBKeyboard::sendKey(int hidCode1, int hidCode2, int hidCode3)
{
	uint8_t data[5]={0xE8, static_cast<uint8_t>(hidCode1), static_cast<uint8_t>(hidCode2), static_cast<uint8_t>(hidCode3), 0xE9};
	sendData(data, 5);
}

//====================================================================

#endif
