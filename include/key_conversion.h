/*********************************************************************
* enum class SPKEYS                                                  *
* class KeyConversion                                                *
* class ComboStringParser                                            *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _KEY_CONVERSION_H
#define _KEY_CONVERSION_H

#include <string> // for uint
#include <map>
#include <functional>

#define MAX_HID_CODES 6 

//====================================================================

enum class SPKEYS
{
	NONE=0,
	ESC,
	SCROLLLOCK,
	PAUSE,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	SYSRQ,
	BACKSPACE,
	ENTER,
	INSERT,
	HOME,
	PAGEUP,
	DELETE,
	END,
	PAGEDOWN,
	UP,
	LEFT,
	DOWN,
	RIGHT,
	CAPSLOCK,
	LEFTCTRL,
	RIGHTCTRL,
	LEFTALT,
	RIGHTALT,
	LEFTSHIFT,
	RIGHTSHIFT,
	LEFTMETA,
	RIGHTMETA,
	_LAST,
};

//--------------------------------------------------------------------

class TinyUSBKeyboard;
class UinputKeyboard;

class KeyConversion
{
	typedef std::function<uint(uint, uint)> ConvCode;

	public:
		template<typename T>
		static uint getKeyCode(SPKEYS keyCode)
		{
			return 0;
		}

	private:
		static uint getKeyCode(SPKEYS keyCode, ConvCode _getKeyCode);
};

template<>
inline uint KeyConversion::getKeyCode<UinputKeyboard>(SPKEYS keyCode)
{
	return getKeyCode(keyCode, [](uint uinputKey, uint tinyusbKey){
		return uinputKey;
	});
}

template<>
inline uint KeyConversion::getKeyCode<TinyUSBKeyboard>(SPKEYS keyCode)
{
	return getKeyCode(keyCode, [](uint uinputKey, uint tinyusbKey){
		return tinyusbKey;
	});
}

//====================================================================

class ComboStringParser final
{
	public:
		ComboStringParser(const std::string& shortcutStr);
		ComboStringParser(const ComboStringParser& other);
		ComboStringParser(ComboStringParser&& other);
		~ComboStringParser();
		ComboStringParser& operator=(const ComboStringParser& other);

		uint size() const;
		const char* getPart(uint i) const;
		bool toKeycode(const std::map<std::string, int>* shortcutParserKeyMapPtr, int (&keyCodes)[MAX_HID_CODES]) const;

	private:
		char* m_str;
		char* m_parts[MAX_HID_CODES];
		uint m_count;

		static bool trim(const std::string& str, size_t offset, size_t& pl, size_t& pr);
};

//--------------------------------------------------------------------

inline ComboStringParser::~ComboStringParser()
{
	if(m_str){
		delete[] m_str;
		m_str=nullptr;
	}
}

//--------------------------------------------------------------------

inline uint ComboStringParser::size() const
{
	return m_count;
}

//--------------------------------------------------------------------

inline const char* ComboStringParser::getPart(uint i) const
{
	if(i<m_count){
		return m_parts[i];
	}
	return nullptr;
}

//====================================================================

#endif
