/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class KeyboardEmulatorI                                            *
* class DummyKeyboard                                                *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _KEYBOARD_EMULATOR_H
#define _KEYBOARD_EMULATOR_H
#include "key_conversion.h"
#include "error_reporting.h"
#include "utf8_text.h"

#include <functional>
#include <map>
#include <thread>

//====================================================================

class KeyboardEmulatorI : public ErrorReporting
{
	typedef std::function<void(KeyboardEmulatorI*)> Combo;

	public:
		~KeyboardEmulatorI()=default;

		virtual bool reload();

		virtual bool isActive()=0;
		virtual void numLk()=0;

		virtual void loadPrintableCharacters()=0;
		
		void enter();
		void inputText(const char8_t* text);
		void inputLine(const char8_t* text);
		void unicodeCharacter(const char8_t* unicode);
		void shortcut(const KeyCombo& shortcut);

		virtual void commandKey(SPKEYS k1)=0;

		const char8_t* getKeyName(int rawKeyCode) const;

	protected:
		const std::map<std::string, int>* m_keyMap;

		KeyboardEmulatorI();
		
		void addCombo(UTF8Char c, std::function<void(KeyboardEmulatorI*)> combo);

		void addCombo(UTF8Char c, int k1);

		void loadPrintableCharacters(const char* fileName);

	private:
		std::map<UTF8Char, Combo> m_combos;
		std::map<int, std::pair<int,std::u8string>> m_shortcuts;

		virtual void sendKey(int keyCode)=0;
		virtual void sendKey(int hidCode1, int hidCode2)=0;
		virtual void sendKey(int hidCode1, int hidCode2, int hidCode3)=0;
		virtual void sendKey(const KeyCombo& keyCodes)=0;

		virtual void addWhiteCharacters()=0;
		virtual void prepareUnicodeInput()=0;

		bool symbolExists(const char8_t* utf8key);

		static std::function<void(KeyboardEmulatorI*)> comboBuilder(const KeyCombo& keyCodes);

		UTF8Char printableCharacterParser(const char* str, std::function<void(KeyboardEmulatorI*)>& combo1, std::function<void(KeyboardEmulatorI*)>& combo2, const std::map<std::string, int>* keyMap);

	friend
	class KeyboardConfigurator;
};

//--------------------------------------------------------------------

// trivial because it is not necessary to be implemente by tinyusb stuff
inline bool KeyboardEmulatorI::reload()
{
	return true;
}

//--------------------------------------------------------------------

inline void KeyboardEmulatorI::enter()
{
	commandKey(SPKEYS::ENTER);
}

//--------------------------------------------------------------------

inline void KeyboardEmulatorI::inputText(const char8_t* text)
{
	clearError();
	UTF8Char symbol;
	UTF8_Text utf8Text(text);
	utf8Text.iterar([this, &symbol](const char8_t* ptr, int bts){
		symbol.loadChar(ptr, bts);
		if(m_combos.find(symbol)!=m_combos.end()){
			m_combos[symbol](this);
		}
		else{
			setErrorCode(1);
		}
	});
}

//--------------------------------------------------------------------

inline void KeyboardEmulatorI::inputLine(const char8_t* text)
{
	inputText(text);
	enter();
}

//--------------------------------------------------------------------

inline void KeyboardEmulatorI::unicodeCharacter(const char8_t* unicode)
{
	prepareUnicodeInput();
	inputLine(unicode);
}

//--------------------------------------------------------------------

inline void KeyboardEmulatorI::addCombo(UTF8Char c, std::function<void(KeyboardEmulatorI*)> combo)
{
	m_combos.emplace(c, combo);
}

//--------------------------------------------------------------------

inline void KeyboardEmulatorI::addCombo(UTF8Char c, int k)
{
	addCombo(c, [k](KeyboardEmulatorI* kboard){
		kboard->sendKey(k);
	});
}

//--------------------------------------------------------------------

inline bool KeyboardEmulatorI::symbolExists(const char8_t* utf8key)
{
	return m_combos.find(utf8key)!=m_combos.end();
}

//--------------------------------------------------------------------

inline const char8_t* KeyboardEmulatorI::getKeyName(int rawKeyCode) const
{
	std::map<int, std::pair<int, std::u8string>>::const_iterator it=m_shortcuts.find(rawKeyCode);
	if(it!=m_shortcuts.end()){
		return it->second.second.c_str();
	}
	return u8"";
}

//====================================================================

class DummyKeyboard : public KeyboardEmulatorI
{
	public:
		DummyKeyboard()=default;
		virtual ~DummyKeyboard()=default;

		virtual void sendKey(int keyCode){}
		virtual void sendKey(int hidCode1, int hidCode2){}
		virtual void sendKey(int hidCode1, int hidCode2, int hidCode3){};
		virtual void sendKey(const KeyCombo& keyCodes){};
		virtual void numLk(){};
		virtual bool isActive(){return false;};

		virtual void loadPrintableCharacters(){};
		virtual void commandKey(SPKEYS k1){};

	private:
		virtual void addWhiteCharacters(){};
		virtual void prepareUnicodeInput(){};
};

//====================================================================

#endif
