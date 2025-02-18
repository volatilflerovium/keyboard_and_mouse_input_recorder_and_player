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

		void clickLeftBtn();
		void clickRightBtn();

		/*
		 *	Move mouse to the relative position (dy, dy) in small steps
		 * */
		void move(const int dx, const int dy);

		/*
		 * Will move the mouse to the absolute position (absX, absY)
		 * */
		virtual void go2Position(const int absX, const int absY, ClientMousePosition getMousePosition);

		/*
		 * Select rectangle: (absX, absY, width, height)
		 * Note that width and height can be negative values
		 * */
		void select(uint absX, uint absY, uint width, uint height, ClientMousePosition getMousePosition);

		/*
		 * Drag the mouse from absolute position (startX, startY)
		 * to absolute position (endX, endY)
		 * */
		void drag(uint startX, uint startY, uint endX, uint endY, ClientMousePosition getMousePosition);
		
		//void scroll(const int x, const int y);

	private:
		// for some reason as the values are bigger there is some lost of presicion
		enum STEP
		{
			LOW=3,
			THR=LOW+1,
		};

		/*
		 * Move the mouse to the relative position (dx, dy)
		 * */
		virtual void setPosition(const int dx, const int dy)=0;
		virtual void buttonDown(MOUSE_BUTTONS btn)=0;
		virtual void buttonUp(MOUSE_BUTTONS btn)=0;

		void moveAbs(const int absX, const int absY, ClientMousePosition getMousePosition);
};

//--------------------------------------------------------------------

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
