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

#include <linux/uinput.h>

//====================================================================

class UinputMouse : public MouseEmulatorI
{
	public:
		UinputMouse();
		virtual ~UinputMouse();
		
	private:
		uinput_setup m_usetup={0};
		input_event m_inputEvent={0};
		int m_fd;

		bool emit(int type, int code, int val);
		void init(const char* deviceName);

		int getMouseButton(const MOUSE_BUTTONS btn);

		/*
		 * Move the mouse to the relative position (dx, dy)
		 * */
		virtual void setPosition(const int dx, const int dy) override;

		virtual void buttonDown(MOUSE_BUTTONS btn) override;
		virtual void buttonUp(MOUSE_BUTTONS btn) override;
};

//--------------------------------------------------------------------

inline int UinputMouse::getMouseButton(const MOUSE_BUTTONS btn)
{
	if(btn==MOUSE_BUTTONS::LEFT){
		return BTN_LEFT;
	}
	return BTN_RIGHT;
}

//--------------------------------------------------------------------

#endif
