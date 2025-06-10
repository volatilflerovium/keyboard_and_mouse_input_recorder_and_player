/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class KeyboardConfigurator                                         *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    31-05-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _KEYBOARD_CONFIGURATOR_H
#define _KEYBOARD_CONFIGURATOR_H
#include "key_index.h"
#include "keycombo.h"
#include "utf8_text.h"

#include <functional>
#include <map>

//====================================================================

class KeyboardConfigurator
{
	public:
		~KeyboardConfigurator();

		static bool symbolExists(const char8_t* utf8key)
		{
			return getInstance()._symbolExists(utf8key);
		}

		static void testCombo(const KeyCombo& keyboardKeyCodes1, const KeyCombo& keyboardKeyCodes2)
		{
			getInstance()._testCombo(keyboardKeyCodes1, keyboardKeyCodes2);
		}

		static void addCombo(const char8_t* utf8key, const KeyCombo& keyboardKeyCodes1, const KeyCombo& keyboardKeyCodes2)
		{
			getInstance()._addCombo(utf8key, keyboardKeyCodes1, keyboardKeyCodes2);
		}

		static void loadSymbols(std::function<void(const char* symbol)> cbk)
		{
			getInstance()._loadSymbols(cbk);
		}

		static bool loadKeyMap()
		{
			return getInstance()._loadKeyMap();
		}

		static void mapper(int keyboardKeyCode)
		{
			getInstance()._mapper(keyboardKeyCode);
		}

		static std::pair<bool, bool> listener(int keyboardKeyCode, const char8_t* symbol)
		{
			return getInstance()._listener(keyboardKeyCode, symbol);
		}

		static int typing()
		{
			return getInstance()._typing();
		}

		static std::pair<bool, bool> listenerComposite(const char8_t* symbol)
		{
			return getInstance()._listenerComposite(symbol);
		}

		static int typingComposite()
		{
			return getInstance()._typingComposite();
		}

	private:
		struct RawCombo
		{
			RawCombo(const char8_t* utf8key, int index);
			RawCombo(const char8_t* utf8key, const KeyCombo& keyCodes);
			RawCombo(const char8_t* utf8key, const KeyCombo& keyCodes1, const KeyCombo& keyCodes2);

			void saveToFile(std::ostream& outputStream);

			KeyCombo m_hidIndexes1;				
			KeyCombo m_hidIndexes2;
			const UTF8Char m_utd8Char;				
		};

		std::map<uint, uint> m_keyboardMapping;
		std::vector<RawCombo> m_printableCharacterCombos;
		std::vector<int> m_printableKeys;
		
		size_t m_keyScanIndex;
		size_t m_currentIndex;

		void addRawCombo(const char8_t* utf8key, int index)
		{
			m_printableCharacterCombos.emplace_back(utf8key, index);
		}

		void addRawCombo(const char8_t* utf8key, const KeyCombo& keyCodes)
		{
			m_printableCharacterCombos.emplace_back(utf8key, keyCodes);
		}

		void addRawCombo(const char8_t* utf8key, const KeyCombo& keyCodes1, const KeyCombo& keyCodes2)
		{
			m_printableCharacterCombos.emplace_back(utf8key, keyCodes1, keyCodes2);
		}

		static KeyboardConfigurator& getInstance()
		{
			static KeyboardConfigurator instance;
			return instance;
		}

		KeyboardConfigurator();

		std::pair<bool, bool> _listenerComposite(const char8_t* symbol);
		int _typingComposite();

		void _testCombo(const KeyCombo& keyboardKeyCodes1, const KeyCombo& keyboardKeyCodes2);
		void _addCombo(const char8_t* utf8key, const KeyCombo& keyboardKeyCodes1, const KeyCombo& keyboardKeyCodes2);
		bool _symbolExists(const char8_t* utf8key);
		void _loadSymbols(std::function<void(const char* symbol)> cbk);
		bool _loadKeyMap();
		void _mapper(int keyboardKeyCode);
		std::pair<bool, bool> _listener(int keyboardKeyCode, const char8_t* symbol);
		int _typing();

		KeyCombo getIndex(const KeyCombo& keyboardKeyCodes);
		KeyCombo getValues(const KeyCombo& indexes);

		void sendKeyCodes(const KeyCombo& keyboardKeyCodes);
		void shortcutMapper(int idx, int keyboardCode, const char* keyName);
		void shortcutMapper(int idx, int keyboardCode, const char8_t* symbol);
		
		int index2EmulatorCode(int index) const;
		int keyboard2EmulatorCode(int rawKeyCode) const;
};

//--------------------------------------------------------------------

inline KeyboardConfigurator::KeyboardConfigurator()
:m_keyScanIndex(0)
, m_currentIndex(0)
{}

//--------------------------------------------------------------------

inline KeyboardConfigurator::RawCombo::RawCombo(const char8_t* utf8key, int index)
:m_hidIndexes1(index)
, m_utd8Char(utf8key)
{}

//--------------------------------------------------------------------

inline KeyboardConfigurator::RawCombo::RawCombo(const char8_t* utf8key, const KeyCombo& keyCodes)
:m_hidIndexes1(keyCodes)
, m_utd8Char(utf8key)
{
}

//--------------------------------------------------------------------

inline KeyboardConfigurator::RawCombo::RawCombo(const char8_t* utf8key, const KeyCombo& keyCodes1, const KeyCombo& keyCodes2)
:m_hidIndexes1(keyCodes1)
, m_hidIndexes2(keyCodes2)
, m_utd8Char(utf8key)
{	
}

//====================================================================

#endif
