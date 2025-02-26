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
#ifndef _UINPUT_KEYBOARD_H
#define _UINPUT_KEYBOARD_H

#include "keyboard_emulator.h"
#include "key_map.h"

#include "key_conversion.h"

#include <linux/uinput.h>

//====================================================================

class UinputKeyboard : public KeyboardEmulatorI
{
	public:
		UinputKeyboard();
		virtual ~UinputKeyboard();

		virtual bool reload();

		virtual void numLk() override;
		virtual bool isActive() override;

		virtual void loadPrintableCharacters() override;
		virtual void commandKey(SPKEYS k) override;

	private:
		uinput_setup m_usetup={0};
		input_event m_inputEvent={0};
		int m_fd;

		bool emit(int type, int code, int val);
		void init(const char* deviceName);

		virtual void sendKey(int keyCode) override;
		virtual void sendKey(int hidCode1, int hidCode2) override;
		virtual void sendKey(int hidCode1, int hidCode2, int hidCode3, int hidCode4=-1, int hidCode5=-1, int hidCode6=-1) override;

		virtual void addWhiteCharacters() override;
		virtual void prepareUnicodeInput() override;
};

//--------------------------------------------------------------------

inline void UinputKeyboard::loadPrintableCharacters()
{
	KeyboardEmulatorI::loadPrintableCharacters("printable_characters.txt", uinputKeyMap);
}

//--------------------------------------------------------------------

inline void UinputKeyboard::commandKey(SPKEYS k)
{
	sendKey(KeyConversion::getKeyCode<UinputKeyboard>(k));
}

//--------------------------------------------------------------------

#endif
