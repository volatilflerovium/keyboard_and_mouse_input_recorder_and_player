/*********************************************************************
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
#include "tinyusb_key_map.h"
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
		virtual void sendKey(int hidCode1, int hidCode2, int hidCode3, int hidCode4=-1, int hidCode5=-1, int hidCode6=-1) override;

		virtual void addWhiteCharacters() override;
		virtual void prepareUnicodeInput() override;

		void sendData(uint8_t* keyCodes, unsigned int N);
};

//--------------------------------------------------------------------

inline void TinyUSBKeyboard::loadPrintableCharacters()
{
	KeyboardEmulatorI::loadPrintableCharacters("tinyusb_printable_characters.txt", tinyusbKeyMap);
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

#endif
