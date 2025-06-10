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
#include "keyboard_emulator.h"
#include "utilities.h"
#include "debug_utils.h"
#include "cstr_split.h"

#include <fstream>

//====================================================================

KeyboardEmulatorI::KeyboardEmulatorI()
{
	if(existPath(SHOTCUT_MAPPING)){
		std::ifstream shortcutMappingFile(getFilePath(SHOTCUT_MAPPING), std::ifstream::in);
		if(shortcutMappingFile.is_open()){
			std::string dataLine;
			while(std::getline(shortcutMappingFile, dataLine)){
				CstrSplit<5> parts(dataLine.c_str(), " ");
				m_shortcuts[std::atoi(parts[0])]={std::atoi(parts[1]), reinterpret_cast<const char8_t*>(parts[2])};
			}
		}
	}
}

//--------------------------------------------------------------------

std::function<void(KeyboardEmulatorI*)> KeyboardEmulatorI::comboBuilder(const KeyCombo& keyCodes)
{	
	int k0=keyCodes[0];

	if(keyCodes[1]<0){
		return [k0](KeyboardEmulatorI* kboard){
			kboard->sendKey(k0);
		};
	}

	int k1=keyCodes[1];
	if(keyCodes[2]<0){
		return [k0, k1](KeyboardEmulatorI* kboard){
			kboard->sendKey(k0, k1);
		};
	}

	int k2=keyCodes[2];
	if(keyCodes[3]<0){
		return [k0, k1, k2](KeyboardEmulatorI* kboard){
			kboard->sendKey(k0, k1);
		};
	}

	return [keyCodes](KeyboardEmulatorI* kboard){
		kboard->sendKey(keyCodes);
	};
}

//--------------------------------------------------------------------

UTF8Char KeyboardEmulatorI::printableCharacterParser(const char* str, std::function<void(KeyboardEmulatorI*)>& combo1, std::function<void(KeyboardEmulatorI*)>& combo2, const std::map<std::string, int>* keyMap)
{
	CstrSplit<3> breaker2(str, PRINT_CHAR_SEPARATOR);

	UTF8Char key(breaker2[0]);

	for(size_t i=1; i<breaker2.dataSize(); i++){
		CstrSplit<MAX_HID_CODES> breaker3(breaker2[i], " ");
		int values[MAX_HID_CODES]={-1, -1, -1, -1, -1, -1};
		for(size_t j=0; j<breaker3.dataSize(); j++){
			std::string tmp(breaker3[j]);
			std::map<std::string, int>::const_iterator it=keyMap->find(tmp);
			
			if(it!=keyMap->end()){
				values[j]=it->second;
			}
		}
		if(i==1){
			combo1=KeyboardEmulatorI::comboBuilder(values);
		}
		else{
			combo2=KeyboardEmulatorI::comboBuilder(values);
		}
	}

	return key;
}

//--------------------------------------------------------------------

void KeyboardEmulatorI::loadPrintableCharacters(const char* fileName)
{
	if(isActive()){
		addWhiteCharacters();

		std::ifstream characterTable(getFilePath(PRINTABLE_CHARACTERS), std::ifstream::in);
		if(!characterTable.is_open()){
			return;
		}

		std::string combo;
		combo.reserve(32);
		while(std::getline(characterTable, combo)){
			if(combo.length()==0){
				continue;
			}

			std::function<void(KeyboardEmulatorI*)> combo1=nullptr;
			std::function<void(KeyboardEmulatorI*)> combo2=nullptr;

			UTF8Char key=printableCharacterParser(combo.c_str(), combo1, combo2, m_keyMap);
			if(combo2){
				addCombo(key, [combo1, combo2](KeyboardEmulatorI* kboard){
					combo1(kboard);
					combo2(kboard);
				});
			}
			else{
				addCombo(key, combo1);
			}
		}
		characterTable.close();
	}
}

//--------------------------------------------------------------------

void KeyboardEmulatorI::shortcut(const KeyCombo& shortcut)
{
	int max=0;
	KeyCombo keyCodes;
	for(int i=0; i<MAX_HID_CODES; i++){
		if(shortcut[i]<0){
			max=i;
			break;
		}
		const std::string& str=keyScan[m_shortcuts[shortcut[i]].first];

		std::map<std::string, int>::const_iterator it=m_keyMap->find(str);
			
		if(it!=m_keyMap->end()){
			keyCodes.pushBack(it->second);
		}
	}
	if(max==1){
		sendKey(keyCodes[0]);
		return;
	}

	if(max==2){
		sendKey(keyCodes[0], keyCodes[1]);
		return;
	}

	if(max==3){
		sendKey(keyCodes[0], keyCodes[1], keyCodes[2]);
		return;
	}

	sendKey(keyCodes);
}

//====================================================================
