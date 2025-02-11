/*********************************************************************
* class KeyboardEmulatorI                                            *
* class DummyKeyboard                                                *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "keyboard_emulator.h"

#include "key_map.h"
#include "utilities.h"
#include "debug_utils.h"

#include <fstream>

//====================================================================

void KeyboardEmulatorI::addCombo(char c, int k1, int k2, int k3, int k4, int k5)
{
	//Let's allow overide of an existing combo
	if(k2<0){
		m_combos[c]=[k1](KeyboardEmulatorI* kboard){
			kboard->sendKey(k1);
		};
		return;
	}

	if(k3<0){
		m_combos[c]=[k1, k2](KeyboardEmulatorI* kboard){
			kboard->sendKey(k1, k2);
		};
		return;
	}
	m_combos[c]=[=](KeyboardEmulatorI* kboard){
		kboard->sendKey(k1, k2, k3, k4, k5);
	};
}

//--------------------------------------------------------------------

static char printableCharacterParser(const std::string& str, int* values, const std::map<std::string, int>& keyMap)
{
	size_t pl=0;
	size_t pr=0;
	const char separator=' ';
	char key=0;
	int k=0;
	
	while(pl!=std::string::npos){
		pl=str.find_first_not_of(separator, pr);
		pr=str.find(separator, pl);
		if(pl!=std::string::npos){
			if(pr-pl==1 && key==0){
				key=str[pl];
			}
			else{
				std::string tmp=str.substr(pl, pr-pl);
				std::map<std::string, int>::const_iterator it=keyMap.find(tmp);
				
				if(it!=keyMap.end()){
					values[k++]=it->second;
				}
			}
		}

		if(pr==std::string::npos){
			return key;
		}
	}
	return 0;
}

//--------------------------------------------------------------------

void KeyboardEmulatorI::loadPrintableCharacters(const char* fileName, const std::map<std::string, int>& keyMap)
{
	if(isActive()){
		addWhiteCharacters();

		std::ifstream characterTable;
		
		std::string filePath=resourcePath(fileName);
		
		characterTable.open(filePath, std::ifstream::in);
		if(!characterTable.is_open()){
			characterTable.close();
			filePath.append(" : not found");
			setLastError([this](){
				return true;
			}, filePath.c_str());
			return;
		}

		std::string combo;
		combo.reserve(32);
		while(std::getline(characterTable, combo)){
			if(combo.length()==0){
				continue;
			}
			int values[5]={-1, -1, -1, -1, -1};
			char c=printableCharacterParser(combo, values, keyMap);
			if(c>0){
				addCombo(c, values[0], values[1], values[2], values[3], values[4]);
			}
		}
		characterTable.close();
	}
}

//====================================================================
