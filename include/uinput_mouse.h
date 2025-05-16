/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class UinputMouse                                                  *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _UINPUT_MOUSE_H
#define _UINPUT_MOUSE_H

#include "mouse_emulator.h"

#include <string>
#include <linux/uinput.h>

//====================================================================

class UinputMouse : public MouseEmulatorI
{
	public:
		UinputMouse();
		virtual ~UinputMouse();

		virtual bool reload();
		
	private:
		uinput_setup m_usetup={0};
		input_event m_inputEvent={0};
		int m_fd;
		const uint16_t c_displayWidth;
		const uint16_t c_displayHeight;

		bool emit(int type, int code, int val);
		void init(const char* deviceName);

		int getMouseButton(const MOUSE_BTN btn);

		/*
		 * Move the mouse to the absolute position (absx, absy)
		 * */
		virtual void setPosition(const int absX, const int absY) override;

		virtual void buttonDown(MOUSE_BTN btn) override;
		virtual void buttonUp(MOUSE_BTN btn) override;
};

//--------------------------------------------------------------------

inline int UinputMouse::getMouseButton(const MOUSE_BTN btn)
{
	if(btn==MOUSE_BTN::LEFT){
		return BTN_LEFT;
	}
	return BTN_RIGHT;
}

//--------------------------------------------------------------------

inline void UinputMouse::buttonDown(MOUSE_BTN button)
{
	emit(EV_KEY, getMouseButton(button), 1);
	emit(EV_SYN, SYN_REPORT, 0);
}

//--------------------------------------------------------------------

inline void UinputMouse::buttonUp(MOUSE_BTN button)
{
	emit(EV_KEY, getMouseButton(button), 0);
	emit(EV_SYN, SYN_REPORT, 0);
}

//--------------------------------------------------------------------

#endif
