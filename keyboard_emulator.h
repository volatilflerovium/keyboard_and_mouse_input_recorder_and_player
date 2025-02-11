/*********************************************************************
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
#include "debug_utils.h"

#include <functional>
#include <map>
#include <thread>
#include <cstring>
#include <string>

//====================================================================

class KeyboardEmulatorI : public ErrorReporting
{
	typedef std::function<void(KeyboardEmulatorI*)> Combo;

	public:
		~KeyboardEmulatorI()=default;

		virtual bool isActive()=0;
		virtual void numLk()=0;

		virtual void loadPrintableCharacters()=0;
		
		void enter();
		void inputText(const char* text);
		void inputLine(const char* text);
		void unicodeCharacter(const char* unicode);
		void shortcut(const char* sct);
		void shortcut(const ComboStringParser& shortcut);

		virtual void commandKey(SPKEYS k1)=0;

	protected:
		const std::map<std::string, int>* m_shortcutParserKeyMapPtr;

		KeyboardEmulatorI()=default;
		
		void addCombo(char c, int k1, int k2=-1, int k3=-1, int k4=-1, int k5=-1);
		void loadPrintableCharacters(const char* fileName, const std::map<std::string, int>& keyMap);

	private:
		std::map<char, Combo> m_combos;

		virtual void sendKey(int keyCode)=0;
		virtual void sendKey(int hidCode1, int hidCode2)=0;
		virtual void sendKey(int hidCode1, int hidCode2, int hidCode3, int hidCode4=-1, int hidCode5=-1, int hidCode6=-1)=0;

		virtual void addWhiteCharacters()=0;
		virtual void prepareUnicodeInput()=0;
};

//--------------------------------------------------------------------

inline void KeyboardEmulatorI::shortcut(const char* sct)
{
	ComboStringParser shortcutObj(sct);
	shortcut(shortcutObj);
}

//--------------------------------------------------------------------

inline void KeyboardEmulatorI::shortcut(const ComboStringParser& shortcut)
{
	int keyCodes[MAX_HID_CODES];
	shortcut.toKeycode(m_shortcutParserKeyMapPtr, keyCodes);
	
	sendKey(keyCodes[0], keyCodes[1], keyCodes[2], keyCodes[3], keyCodes[4], keyCodes[5]);
}

//--------------------------------------------------------------------

inline void KeyboardEmulatorI::enter()
{
	commandKey(SPKEYS::ENTER);
}

//--------------------------------------------------------------------

inline void KeyboardEmulatorI::inputText(const char* text)
{
	for(size_t i=0; i<std::strlen(text); i++){
		if(m_combos.find(text[i])!=m_combos.end()){
			m_combos[text[i]](this);
		}
	}
}

//--------------------------------------------------------------------

inline void KeyboardEmulatorI::inputLine(const char* text)
{
	inputText(text);
	enter();
}

//--------------------------------------------------------------------

inline void KeyboardEmulatorI::unicodeCharacter(const char* unicode)
{
	prepareUnicodeInput();
	std::this_thread::sleep_for(std::chrono::milliseconds(15));
	inputLine(unicode);
}

//====================================================================

class DummyKeyboard : public KeyboardEmulatorI
{
	public:
		DummyKeyboard()=default;
		virtual ~DummyKeyboard()=default;

		virtual void sendKey(int keyCode){}
		virtual void sendKey(int hidCode1, int hidCode2){}
		virtual void sendKey(int hidCode1, int hidCode2, int hidCode3, int hidCode4=-1, int hidCode5=-1, int hidCode6=-1){}

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
