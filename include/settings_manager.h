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
#ifndef _SETTINGS_MANAGER_H
#define _SETTINGS_MANAGER_H

#include "utilities.h"
#include "cstr_split.h"

#include "hid_manager.h"
#include "enumerations.h"

#include <fstream>
#include <wx/string.h>
#include <wx/msgdlg.h>

#define SETTINGS_FILE ".save_settings.txt"

//====================================================================

class SettingsManager final
{
	public:
		~SettingsManager()
		{
			save();
		}

		static SettingsManager& getSettingManager()
		{
			static SettingsManager manager;
			return manager;
		}

		void save();

		const wxString& alpha() const
		{
			if(isUDP()){
				return m_ip;
			}
			return m_serialPort;
		}
		
		const wxString& alpha(int i) const 
		{
			if(InterfaceLink::UDP==InterfaceLink(i)){
				return m_ip;
			}
			return m_serialPort;
		}

		const uint numeric() const
		{
			if(isUDP()){
				return m_port;
			}
			return m_baudRate;
		}

		const uint& numeric(int i) const 
		{
			if(InterfaceLink::UDP==InterfaceLink(i)){
				return m_port;
			}
			return m_baudRate;
		}

		void saveToFile(std::ostream& outputStream);

		bool loadSettings();

		bool isValidInterface() const
		{
			return InterfaceLink::NONE!=m_interface;
		}

		bool isTinyusbLink() const
		{
			return m_interface==InterfaceLink::UDP || m_interface==InterfaceLink::SERIAL;
		}

		bool isSerial() const
		{
			return m_interface==InterfaceLink::SERIAL;
		}

		bool isUDP() const
		{
			return m_interface==InterfaceLink::UDP;
		}

		bool isSameInterface(int inter) const;

		void setInterfaceSetting(int interface, const wxString& alpha, int number);

		wxString getBrushColour() const
		{
			return m_brushColour;
		}

		void setBrushColour(const wxString& colour)
		{
			m_brushColour=colour;
		}

		wxString getSerialPort() const
		{
			return m_serialPort;
		}

		void setSerialPort(const wxString& port)
		{
			m_serialPort=port;
		}
		
		wxString getIP() const
		{
			return m_ip;
		}

		void setIP(const wxString& ip)
		{
			m_ip=ip;
		}

		uint getTimeDelay() const
		{
			return m_timeDelay;
		}

		void setTimeDelay(uint timeDelay)
		{
			m_timeDelay=timeDelay;
		}

		uint getTimePadding() const
		{
			return m_timePadding;
		}

		void setTimePadding(uint padding)
		{
			m_timePadding=padding;
		}
		
		uint getTransparency() const
		{
			return m_transparency;
		}

		void setTransparency(uint transparency)
		{
			m_transparency=transparency;
		}

		uint getScreenshotTimeout() const
		{
			return m_screenshotTimeout;
		}

		void setScreenshotTimeout(uint timeout)
		{
			m_screenshotTimeout=timeout;
		}

		template<typename T=InterfaceLink>
		T getInterface() const
		{
			return m_interface;
		}

		uint getBaudRate() const
		{
			return m_baudRate;
		}

		void setBaudRate(uint baud)
		{
			m_baudRate=baud;
		}
		
		uint getUDPPort() const
		{
			return m_port;
		}

		void setUDPPort(uint port)
		{
			m_port=port;
		}

	private:
		wxString m_brushColour{"#0000FF"};
		wxString m_serialPort{""};
		wxString m_ip{""};
		uint m_timeDelay{1000};
		uint m_timePadding{600};
		uint m_transparency{31};
		uint m_screenshotTimeout{500};
		InterfaceLink m_interface{InterfaceLink::NONE};
		uint m_baudRate{0};
		uint m_port{0};

		SettingsManager()=default;
};

//--------------------------------------------------------------------

inline void SettingsManager::saveToFile(std::ostream& outputStream)
{
	outputStream<<m_timeDelay<<":"<<m_timePadding<<":"
					<<m_transparency<<":"
					<<m_screenshotTimeout<<":"
					<<m_brushColour<<":"
					<<uint(m_interface)<<":"
					<<m_serialPort<<":"
					<<m_baudRate<<":"
					<<m_ip<<":"
					<<m_port<<": : :\n";
}

//--------------------------------------------------------------------

inline bool SettingsManager::isSameInterface(int inter) const
{
	if(inter<int(InterfaceLink::_LAST) || inter>=int(InterfaceLink::NONE)){
		return m_interface==InterfaceLink(inter);
	}
	return false;
}

//--------------------------------------------------------------------

template<>
inline uint SettingsManager::getInterface<uint>() const
{
	return uint(m_interface);
}

//====================================================================

#endif
