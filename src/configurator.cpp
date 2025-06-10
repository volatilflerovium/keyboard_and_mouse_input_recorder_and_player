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
#include "configurator.h"
#include "keyboard_emulator.h"
#include "utilities.h"
#include "debug_utils.h"
#include "cstr_split.h"

#include <fstream>

extern KeyboardEmulatorI* s_KeyboardEmulator;

//====================================================================

KeyboardConfigurator::~KeyboardConfigurator()
{
	if(m_printableCharacterCombos.size()>0){
		std::fstream fileData(getFilePath(PRINTABLE_CHARACTERS).c_str(), std::ios::out | std::ios_base::app);
		if(fileData.is_open()){
			for(auto& data : m_printableCharacterCombos){
				data.saveToFile(fileData);
			}
			fileData.close();
		}
	}

	if(!existPath(KEYBOARD_MAPPING)){
		std::fstream keyboardMapFile(getFilePath(KEYBOARD_MAPPING).c_str(), std::ios::out | std::ios::trunc);
		if(keyboardMapFile.is_open()){
			for(auto& [k, v] : m_keyboardMapping){
				keyboardMapFile<<k<<" "<<v<<"\n";
			}
			keyboardMapFile.close();
		}
	}

	if(!existPath(SHOTCUT_MAPPING)){
		std::fstream shortcutMapFile(getFilePath(SHOTCUT_MAPPING).c_str(), std::ios::out | std::ios::trunc);
		if(shortcutMapFile.is_open()){
			for(auto& [k, v] : s_KeyboardEmulator->m_shortcuts){
				shortcutMapFile<<k<<" "<<v.first<<" "<<reinterpret_cast<const char*>(v.second.c_str())<<"\n";
			}
			shortcutMapFile.close();
		}
	}
}

//--------------------------------------------------------------------

void KeyboardConfigurator::_loadSymbols(std::function<void(const char* symbol)> cbk)
{
	if(existPath(PRINTABLE_CHARACTERS)){
		std::ifstream symbolFile(getFilePath(PRINTABLE_CHARACTERS), std::ifstream::in);
		if(symbolFile.is_open()){
			std::string dataLine;
			while(std::getline(symbolFile, dataLine)){
				CstrSplit<1> breaker(dataLine.c_str(), PRINT_CHAR_SEPARATOR);

				if(breaker.chunkSize(0)>0){
					cbk(breaker[0]);
				}
			}
		}
	}
}

//--------------------------------------------------------------------

bool KeyboardConfigurator::_loadKeyMap()
{
	m_keyScanIndex=0;
	
	if(existPath(KEYBOARD_MAPPING)){
		std::ifstream keyboardMappingFile(getFilePath(KEYBOARD_MAPPING), std::ifstream::in);
		if(keyboardMappingFile.is_open()){
			std::string dataLine;
			while(std::getline(keyboardMappingFile, dataLine)){
				CstrSplit<2> parts(dataLine.c_str(), " ");
				m_keyboardMapping[std::atoi(parts[0])]=std::atoi(parts[1]);
			}
		}
		return true;
	}
	return false;
}

//--------------------------------------------------------------------

void KeyboardConfigurator::_mapper(int keyboardKeyCode)
{
	if(keyboardKeyCode>-1){
		if(m_keyboardMapping.find(keyboardKeyCode)==m_keyboardMapping.end()){
			m_keyboardMapping[keyboardKeyCode]=m_keyScanIndex-1;
		}
	}
}

//--------------------------------------------------------------------

bool KeyboardConfigurator::_symbolExists(const char8_t* utf8key)
{
	return s_KeyboardEmulator->symbolExists(utf8key);
}

//--------------------------------------------------------------------

std::pair<bool, bool> KeyboardConfigurator::_listener(int keyboardKeyCode, const char8_t* symbol)
{
	int targetIndex=m_keyScanIndex-1;
	bool validSymbol=false;
	if(keyboardKeyCode>-1){
		UTF8Char utf8Symbol(symbol);
		if(utf8Symbol.isValid() && !utf8Symbol.isNullChar()){
			if(s_KeyboardEmulator->m_combos.find(utf8Symbol)==s_KeyboardEmulator->m_combos.end()){

				int keyCode=index2EmulatorCode(targetIndex);
		
				s_KeyboardEmulator->addCombo(utf8Symbol, keyCode);
				addRawCombo(symbol, targetIndex);

				shortcutMapper(targetIndex, keyboardKeyCode, symbol);
				validSymbol=true;
				m_printableKeys.push_back(targetIndex);
			}
		}
		else{
			shortcutMapper(targetIndex, keyboardKeyCode, keyScan[targetIndex].c_str());
		}
	}
	return {m_keyScanIndex==_STOP, validSymbol};
}

//--------------------------------------------------------------------

int KeyboardConfigurator::_typing()
{
	if(m_keyScanIndex<_STOP){

		int keyCode=index2EmulatorCode(m_keyScanIndex++);
		
		if(keyCode>-1){
			s_KeyboardEmulator->sendKey(keyCode);
		}
	}

	return m_keyScanIndex;
}

//--------------------------------------------------------------------

std::pair<bool, bool> KeyboardConfigurator::_listenerComposite(const char8_t* symbol)
{
	static int shiftCode=index2EmulatorCode(SHIFT_LEFT);
	bool validSymbol=false;
	int index=m_currentIndex-1;

	UTF8Char utf8Symbol(symbol);
	if(utf8Symbol.isValid() && !utf8Symbol.isNullChar()){
		if(s_KeyboardEmulator->m_combos.find(utf8Symbol)==s_KeyboardEmulator->m_combos.end()){
			{
				KeyCombo keyCodes;
				keyCodes.pushBack(SHIFT_LEFT);
				keyCodes.pushBack(m_printableKeys[index]);
				addRawCombo(symbol, keyCodes);
			}

			{
				KeyCombo keyCodes;
				
				keyCodes.pushBack(shiftCode);
				keyCodes.pushBack(index2EmulatorCode(m_printableKeys[index]));

				s_KeyboardEmulator->addCombo(
					utf8Symbol,
					s_KeyboardEmulator->comboBuilder(keyCodes)
				);
			}

			validSymbol=true;
		}
	}

	return {m_currentIndex==m_printableKeys.size(), validSymbol};
}

//--------------------------------------------------------------------

int KeyboardConfigurator::_typingComposite()
{
	static int shiftCode=index2EmulatorCode(SHIFT_LEFT);
	if(m_currentIndex<m_printableKeys.size()){

		int keyCode=index2EmulatorCode(m_printableKeys[m_currentIndex++]);

		s_KeyboardEmulator->sendKey(shiftCode, keyCode);
	}
	return m_currentIndex;
}

//--------------------------------------------------------------------

void KeyboardConfigurator::_testCombo(const KeyCombo& keyboardKeyCodes1, const KeyCombo& keyboardKeyCodes2)
{
	sendKeyCodes(keyboardKeyCodes1);
	sendKeyCodes(keyboardKeyCodes2);
}

//--------------------------------------------------------------------

void KeyboardConfigurator::_addCombo(const char8_t* utf8key, const KeyCombo& keyboardKeyCodes1, const KeyCombo& keyboardKeyCodes2)
{
	if(s_KeyboardEmulator->m_combos.find(utf8key)==s_KeyboardEmulator->m_combos.end()){
		std::function<void(KeyboardEmulatorI*)> combo1=nullptr;
		std::function<void(KeyboardEmulatorI*)> combo2=nullptr;

		KeyCombo indexes1=getIndex(keyboardKeyCodes1);
		KeyCombo values1=getValues(indexes1);

		combo1=KeyboardEmulatorI::comboBuilder(values1);
	
		if(keyboardKeyCodes2.isValid()){
			KeyCombo indexes2=getIndex(keyboardKeyCodes2);
			KeyCombo values2=getValues(indexes2);

			if(values1.isValid() && values2.isValid()){
				combo2=KeyboardEmulatorI::comboBuilder(values2);
				s_KeyboardEmulator->addCombo(utf8key, [combo1, combo2](KeyboardEmulatorI* kbrd){
					combo1(kbrd);
					combo2(kbrd);
				});
				addRawCombo(utf8key, indexes1, indexes2);
			}
		}
		else{
			if(values1.isValid()){
				s_KeyboardEmulator->addCombo(utf8key, combo1);
				addRawCombo(utf8key, indexes1);
			}
		}
	}
}

//--------------------------------------------------------------------

KeyCombo KeyboardConfigurator::getIndex(const KeyCombo& keyboardKeyCodes)
{
	KeyCombo indexes;
	for(int i=0; i<MAX_HID_CODES; i++){
		if(keyboardKeyCodes[i]==-1){
			break;
		}

		indexes.pushBack(m_keyboardMapping[keyboardKeyCodes[i]]);
	}

	return indexes;
}

//--------------------------------------------------------------------

KeyCombo KeyboardConfigurator::getValues(const KeyCombo& indexes)
{
	KeyCombo values;
	for(int i=0; i<MAX_HID_CODES; i++){
		if(indexes[i]==-1){
			break;
		}

		values.pushBack(index2EmulatorCode(indexes[i]));
	}
	return values;
}

//--------------------------------------------------------------------

void KeyboardConfigurator::sendKeyCodes(const KeyCombo& keyboardKeyCodes)
{
	KeyCombo values=getValues(getIndex(keyboardKeyCodes));
	if(values[0]!=-1){
		s_KeyboardEmulator->sendKey(values);
	}
}

//====================================================================

void KeyboardConfigurator::RawCombo::saveToFile(std::ostream& outputStream)
{
	outputStream<<m_utd8Char<<PRINT_CHAR_SEPARATOR;

	const int* hidIndexes=m_hidIndexes1.m_data;

	for(int k=0; k<2; k++){
		for(int i=0; i<MAX_HID_CODES; i++){
			if(hidIndexes[i]==-1){
				break;
			}
			if(i>0){
				outputStream<<" ";
			}
			outputStream<<keyScan[hidIndexes[i]];
		}
		
		if(m_hidIndexes2[0]>-1){
			hidIndexes=m_hidIndexes2.m_data;
			outputStream<<PRINT_CHAR_SEPARATOR;
		}
		else{
			break;
		}
	}

	outputStream<<"\n";
}

//--------------------------------------------------------------------

int KeyboardConfigurator::index2EmulatorCode(int index) const
{
	const std::string& identifier=keyScan[index];
	std::map<std::string, int>::const_iterator it=
					s_KeyboardEmulator->m_keyMap->find(identifier);
	if(it!=s_KeyboardEmulator->m_keyMap->end()){
		return it->second;
	}
	else{
		dbg("key: ", identifier, " not found");
	}
	return -1;
}

//--------------------------------------------------------------------

int KeyboardConfigurator::keyboard2EmulatorCode(int rawKeyCode) const
{
	std::map<uint, uint>::const_iterator keyIter=m_keyboardMapping.find(rawKeyCode);
	const std::string& identifier=keyScan[keyIter->second];
	std::map<std::string, int>::const_iterator it=
					s_KeyboardEmulator->m_keyMap->find(identifier);
	if(it!=s_KeyboardEmulator->m_keyMap->end()){
		return it->second;
	}
	else{
		dbg("key: ", identifier, " not found");
	}
	return -1;
}

//--------------------------------------------------------------------

void KeyboardConfigurator::shortcutMapper(int idx, int keyboardCode, const char8_t* symbol)
{
	s_KeyboardEmulator->m_shortcuts[keyboardCode]={idx, symbol};
}

//--------------------------------------------------------------------

void KeyboardConfigurator::shortcutMapper(int idx, int keyboardCode, const char* keyName)
{
	if(idx==ALT_LEFT){
		s_KeyboardEmulator->m_shortcuts[keyboardCode]={idx, u8"<alt-l>"};
	}
	else if(idx==ALT_RIGHT){
		s_KeyboardEmulator->m_shortcuts[keyboardCode]={idx, u8"<alt-r>"};
	}
	else if(idx==CONTROL_LEFT){
		s_KeyboardEmulator->m_shortcuts[keyboardCode]={idx, u8"<ctrl-l>"};
	}
	else if(idx==CONTROL_RIGHT){
		s_KeyboardEmulator->m_shortcuts[keyboardCode]={idx, u8"<ctrl-r>"};
	}
	else if(idx==META_LEFT){
		s_KeyboardEmulator->m_shortcuts[keyboardCode]={idx, u8"<win-l>"};
	}
	else if(idx==META_RIGHT){
		s_KeyboardEmulator->m_shortcuts[keyboardCode]={idx, u8"<win-r>"};
	}
	else if(idx==SHIFT_LEFT){
		s_KeyboardEmulator->m_shortcuts[keyboardCode]={idx, u8"<shift-l>"};
	}
	else if(idx==SHIFT_RIGHT){
		s_KeyboardEmulator->m_shortcuts[keyboardCode]={idx, u8"<shift-r>"};
	}
	else{
		s_KeyboardEmulator->m_shortcuts[keyboardCode]={idx, reinterpret_cast<const char8_t*>(keyName)};
	}
}

//====================================================================
