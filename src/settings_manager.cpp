/*********************************************************************
* class SettingsManager                                              *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "settings_manager.h"

#include <filesystem>

#include <wx/brush.h>

extern wxColour s_colour;
extern wxBrush s_brush;

//====================================================================

bool SettingsManager::loadSettings()
{
	std::error_code ec;
	std::string filePath=getFilePath(SETTINGS_FILE);
	if(std::filesystem::exists(filePath, ec)){
		std::ifstream settingsFile;
		settingsFile.open(filePath.c_str(), std::ifstream::in);
		if(settingsFile.is_open()){
			std::string infoLine;
			while(std::getline(settingsFile, infoLine)){
				if(infoLine.length()==0){
					continue;
				}
				CstrSplit<10> parts(infoLine.c_str(), ":");
				try{
					m_timeDelay=std::atoi(parts[0]);
					m_timePadding=std::atoi(parts[1]);
					m_transparency=std::atoi(parts[2]);
					m_screenshotTimeout=std::atoi(parts[3]);
					m_brushColour=parts[4];
					m_interface=InterfaceLink(std::atoi(parts[5]));
					m_serialPort=parts[6];
					m_baudRate=std::atoi(parts[7]);
					m_ip=parts[8];
					m_port=std::atoi(parts[9]);
					break;
				}
				catch(...)
				{
					continue;
				}
			}
			settingsFile.close();
			s_colour=wxColor(m_brushColour);
			s_brush=wxBrush(s_colour, wxBRUSHSTYLE_CROSSDIAG_HATCH);
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------

void SettingsManager::save()
{
	std::string filePath=getFilePath(SETTINGS_FILE);
	std::fstream fileData(filePath.c_str(), std::ios::out | std::ios::trunc);
	if(fileData.is_open()){
		saveToFile(fileData);
		fileData.close();
	}
	else{
		wxMessageBox(wxT("Save file failed."));
	}
}

//--------------------------------------------------------------------

void SettingsManager::setInterfaceSetting(int interface, const wxString& alpha, int number)
{
	if(interface<int(InterfaceLink::_LAST) || interface>=int(InterfaceLink::NONE)){
		m_interface=InterfaceLink(interface);
		if(isTinyusbLink()){
			if(isSerial()){
				m_serialPort=alpha;
				m_baudRate=number;
			}
			else if(isUDP()){
				m_ip=alpha;
				m_port=number;
			}
		}

		HIDManager::SetHidEmulator(m_interface, this->alpha().mb_str(), numeric(), isSerial());
	}
}

//====================================================================

