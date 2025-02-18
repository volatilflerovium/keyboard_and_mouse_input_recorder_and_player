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
#include "mouse_emulator.h"

#include "utilities.h"
#include "debug_utils.h"

#include <thread>

#define MAX_TRIES 3000

//====================================================================

void MouseEmulatorI::clickLeftBtn()
{
	buttonDown(MOUSE_BUTTONS::LEFT);
	buttonUp(MOUSE_BUTTONS::LEFT);
	std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

//--------------------------------------------------------------------

void MouseEmulatorI::clickRightBtn()
{
	buttonDown(MOUSE_BUTTONS::RIGHT);
	buttonUp(MOUSE_BUTTONS::RIGHT);
	std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

//--------------------------------------------------------------------

void MouseEmulatorI::move(const int x, const int y)
{
	const int sigX=x<0? -1 : 1;
	const int sigY=y<0? -1 : 1;

	int relX=sigX*x;
	int relY=sigY*y;
	int ax=0;
	int ay=0;
	int tries=0;
	while(true && tries++<MAX_TRIES){
		ax=0;
		if(relX>LOW){
			ax=sigX*THR;			
			relX-=THR;
		}

		ay=0;
		if(relY>LOW){
			ay=sigY*THR;		
			relY-=THR;
		}

		setPosition(ax, ay);
		
		if(relX<THR && relY<THR){
			break;
		}
	}

	if(relX>0 || relY>0){
		ax=sigX*relX;
		ay=sigY*relY;
		setPosition(ax, ay);
	}
}

//--------------------------------------------------------------------

void MouseEmulatorI::go2Position(const int absX, const int absY, ClientMousePosition getMousePosition)
{
	int pX;
	int pY;
	getMousePosition(pX, pY);
	int rX=absX-pX;
	int rY=absY-pY;
	int tries=0;
	while((std::abs(rX/2)>0 || std::abs(rY/2)>0) && tries++<MAX_TRIES){
		setPosition(rX/2, rY/2);
		getMousePosition(pX, pY);
		rX=absX-pX;
		rY=absY-pY;
	}

	setPosition(rX, rY);
}

//--------------------------------------------------------------------

void MouseEmulatorI::select(uint absX, uint absY, uint width, uint height, ClientMousePosition getMousePosition)
{
	dbg(absX, " + ", width, " : ", absY, " + ", height);
	go2Position(absX, absY, getMousePosition);
	buttonDown(MOUSE_BUTTONS::LEFT);
	std::this_thread::sleep_for(std::chrono::milliseconds(25));
	moveAbs(absX+width, absY+height, getMousePosition);
	buttonUp(MOUSE_BUTTONS::LEFT);
}

//--------------------------------------------------------------------

void MouseEmulatorI::drag(uint startX, uint startY, uint endX, uint endY, ClientMousePosition getMousePosition)
{
	go2Position(startX, startY, getMousePosition);
	buttonDown(MOUSE_BUTTONS::LEFT);
	std::this_thread::sleep_for(std::chrono::milliseconds(25));
	moveAbs(endX, endY, getMousePosition);
	buttonUp(MOUSE_BUTTONS::LEFT);
}
//--------------------------------------------------------------------

void MouseEmulatorI::moveAbs(const int absX, const int absY, ClientMousePosition getMousePosition)
{
	int pX;
	int pY;
	getMousePosition(pX, pY);
	int relX=absX-pX;
	int relY=absY-pY;
	int sigX=relX<0? -1 : 1;
	int sigY=relY<0? -1 : 1;
	int ax, ay;
	int tries=0;
	while(true && tries++<MAX_TRIES){
		ax=0;
		if(sigX*relX>LOW){
			ax=sigX*THR;
			relX=sigX*std::abs(sigX*relX-THR);
		}

		ay=0;
		if(sigY*relY>LOW){
			ay=sigY*THR;		
			relY=sigY*std::abs(sigY*relY-THR);
		}

		setPosition(ax, ay);
		getMousePosition(pX, pY);
		relX=absX-pX;
		relY=absY-pY;
		sigX=relX<0? -1 : 1;
		sigY=relY<0? -1 : 1;

		if(sigX*relX<THR && sigY*relY<THR){
			break;
		}
	}

	setPosition(relX, relY);
}

//====================================================================
