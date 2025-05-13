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

void MouseEmulatorI::clickBtn(const MOUSE_BUTTONS btn, uint pressForMs)
{
	buttonDown(btn);
	if(pressForMs>0){
		std::this_thread::sleep_for(std::chrono::milliseconds(pressForMs));
	}
	buttonUp(btn);
	std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

//--------------------------------------------------------------------

void MouseEmulatorI::moveFromTo(const int startX, const int startY, const int endX, const int endY)
{
	const int sigX=endX-startX<0? -1 : 1;
	const int sigY=endY-startY<0? -1 : 1;

	int relX=sigX*(endX-startX);
	int relY=sigY*(endY-startY);
	int ax=startX;
	int ay=startY;
	int tries=0;
	while(true && tries++<MAX_TRIES){
		if(relX>LOW){
			ax+=sigX*THR;			
			relX-=THR;
		}

		if(relY>LOW){
			ay+=sigY*THR;		
			relY-=THR;
		}

		setPosition(ax, ay);
		if(relX<THR && relY<THR){
			break;
		}
	}

	if(relX>0 || relY>0){
		setPosition(endX, endY);
	}
}

//--------------------------------------------------------------------

void MouseEmulatorI::go2Position(const int absX, const int absY)
{
	setPosition(absX, absY);
}

//--------------------------------------------------------------------

void MouseEmulatorI::select(uint absX, uint absY, uint width, uint height)
{
	drag(absX, absY, absX+width, absY+height);
}

//--------------------------------------------------------------------

void MouseEmulatorI::drag(uint startX, uint startY, uint endX, uint endY)
{
	go2Position(startX, startY);
	buttonDown(MOUSE_BUTTONS::LEFT);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	moveFromTo(startX, startY, endX, endY);
	buttonUp(MOUSE_BUTTONS::LEFT);
}

//====================================================================
