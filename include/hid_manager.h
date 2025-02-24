/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class HIDManager                                                   *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _HID_MANAGER_H
#define _HID_MANAGER_H

#include "enumerations.h"

#include <functional>

//====================================================================

class HIDManager
{
	public:
		HIDManager()=delete;
		~HIDManager()=default;

		static void SetHidEmulator(const InterfaceLink target, const char* portPath="", uint baudRate=0, bool isSerial=false);
		static bool currentEmulator(HID_TARGET target);
		static int connectionError();
		static const char* verboseError(int errorCode);

	private:
		static HID_TARGET s_currentTarget;
		static bool s_isSerial;

		static void SetDummyEmulator();
		static void SetUinputEmulator();
		static void SetTinyusbEmulator(const char* alpha, uint numeric, bool isSerial);
};

//--------------------------------------------------------------------

inline bool HIDManager::currentEmulator(HID_TARGET target)
{
	return s_currentTarget==target;
}

//--------------------------------------------------------------------

inline void HIDManager::SetHidEmulator(const InterfaceLink target, const char* portPath, uint baudRate, bool isSerial)
{
	if(target==InterfaceLink::NONE){
		SetDummyEmulator();
	}
	else if(target==InterfaceLink::UINPUT){
		SetUinputEmulator();
	}
	else{
		SetTinyusbEmulator(portPath, baudRate, isSerial);
	}
}

//--------------------------------------------------------------------

#endif
