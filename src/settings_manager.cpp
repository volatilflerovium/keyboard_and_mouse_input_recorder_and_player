/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class SettingsManager                                              *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "settings_manager.h"
#include "utilities.h"

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

				SimpleUnserialization<22> cmdData(infoLine.c_str(), SEPARATOR);
				try{
					m_timeDelay=cmdData.get<int>("timeDelay"); 
					m_timePadding=cmdData.get<int>("timePadding"); 
					m_transparency=cmdData.get<int>("transparency"); 
					m_screenshotTimeout=cmdData.get<int>("screenshotTimeout"); 
					m_brushColour=cmdData.get<const char*>("brushColour"); 
					m_interface=InterfaceLink(cmdData.get<int>("interface")); 
					m_serialPort=cmdData.get<const char*>("serialPort"); 
					m_baudRate=cmdData.get<int>("baudRate"); 
					m_ip=cmdData.get<const char*>("ip"); 
					m_port=cmdData.get<int>("port"); 
					m_doubleClick=cmdData.get<int>("doubleClick");
					m_installedApp=InstallationStatus(cmdData.get<int>("installedApp", int(InstallationStatus::INITIAL)));
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

//--------------------------------------------------------------------

bool SettingsManager::appIsInstalled() const
{
#ifndef DEBUG
	std::string appImagePath=getenv("APPIMAGE");
	std::string appimageName=appImagePath.substr(appImagePath.find_last_of("/")+1);

	const wxString desktopEntryFile=wxString::Format("%s/.local/share/applications/kmRecorderPlayer.desktop", getenv("HOME")); 
	const wxString app=wxString::Format("%s/bin/kmRecPlayer/%s", getenv("HOME"), appimageName); 
#else
	const wxString desktopEntryFile="/tmp/kmRecorderPlayer.desktop";
	const wxString app="/tmp/bin/kmRecPlayer/kmRecorderAndPlayer-x86_64.AppImage";
#endif
	
	std::error_code ec;
	bool installed=std::filesystem::exists(std::string(desktopEntryFile.mb_str()), ec);
	if(installed){
		installed=std::filesystem::exists(std::string(app.mb_str()), ec);
	}
	
	return installed;
}

//====================================================================

