/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class MouseEmulatorI                                               *
* class DummyMouse                                                   *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _MOUSE_EMULATOR_H
#define _MOUSE_EMULATOR_H

#include "error_reporting.h"
#include <functional>

#define MAX_DISPLAY_SIZE 0x7FFF /* 32767 This is the value tinyusb provides */

//====================================================================

class MouseEmulatorI : public ErrorReporting
{
	public:
		typedef std::function<void(int&, int&)> ClientMousePosition;

		enum MOUSE_BUTTONS
		{
			LEFT  =0,
			RIGHT =1,
		};
		
	public:
		MouseEmulatorI()=default;
		virtual ~MouseEmulatorI()=default;

		virtual bool reload();

		void clickLeftBtn(uint pressForMs=0);
		void clickRightBtn(uint pressForMs=0);

		/*
		 *	Move mouse to the absolute position (endX, endY) in small steps
		 * */
		void moveFromTo(const int startX, const int startY, const int endX, const int endY);

		/*
		 * Move the mouse to the absolute position (absX, absY)
		 * */
		virtual void go2Position(const int absX, const int absY);

		/*
		 * Select rectangle: (absX, absY, width, height)
		 * Note that width and height can be negative values
		 * */
		void select(uint absX, uint absY, uint width, uint height);

		/*
		 * Drag the mouse from absolute position (startX, startY)
		 * to absolute position (endX, endY)
		 * */
		void drag(uint startX, uint startY, uint endX, uint endY);
		
		//void scroll(const int x, const int y);

	private:
		enum STEP
		{
			LOW=3,
			THR=LOW+1,
		};

		void clickBtn(const MOUSE_BUTTONS btn, uint pressForMs);

		/*
		 * Move the mouse to the relative position (dx, dy)
		 * */
		virtual void setPosition(const int dx, const int dy)=0;
		virtual void buttonDown(MOUSE_BUTTONS btn)=0;
		virtual void buttonUp(MOUSE_BUTTONS btn)=0;
};

//--------------------------------------------------------------------

// trivial because it is not necessary to be implemente by tinyusb stuff
inline bool MouseEmulatorI::reload()
{
	return true;
}

//--------------------------------------------------------------------

inline void MouseEmulatorI::clickLeftBtn(uint pressForMs)
{
	clickBtn(MOUSE_BUTTONS::LEFT, pressForMs);
}

//--------------------------------------------------------------------

inline void MouseEmulatorI::clickRightBtn(uint pressForMs)
{
	clickBtn(MOUSE_BUTTONS::RIGHT, pressForMs);
}

//====================================================================

class DummyMouse : public MouseEmulatorI
{
	public:
		DummyMouse()=default;
		virtual ~DummyMouse()=default;

	private:
		virtual int getMouseButton(const MOUSE_BUTTONS btn){return 0;}
		virtual void setPosition(const int dx, const int dy){}
		virtual void buttonDown(MOUSE_BUTTONS btn){}
		virtual void buttonUp(MOUSE_BUTTONS btn){}
};

//====================================================================

#endif
