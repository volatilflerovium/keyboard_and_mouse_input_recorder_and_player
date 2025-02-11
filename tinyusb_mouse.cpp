/*********************************************************************
* class TinyusbMouse                                                 *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "tinyusb_mouse.h"
#include "debug_utils.h"
#include <cstdint>

//====================================================================

TinyusbMouse::TinyusbMouse()
:m_button()
{
	setLastError([this](){
		return !s_connector->isActive();
	});
}

//--------------------------------------------------------------------

void TinyusbMouse::sendMouseData(int8_t deltaX, int8_t deltaY, int8_t scrollV, int8_t scrollH)
{
	int8_t data[6];

	data[0]=MOUSE_MESSAGE;
	data[1]=m_button;
	data[2]=static_cast<int8_t>(deltaX);
	data[3]=static_cast<int8_t>(deltaY);
	data[4]=static_cast<int8_t>(scrollV);
	data[5]=static_cast<int8_t>(scrollH);
		
	sendAndWait(data, 6);
}

//--------------------------------------------------------------------

void TinyusbMouse::setPosition(const int dx, const int dy)
{
	// This does not need to be smooth. MouseEmulatorI::move is already smooth 
	const int sigX=dx<0? -1 : 1;
	const int sigY=dy<0? -1 : 1;

	int x=sigX*dx;
	int y=sigY*dy;
	int tx;
	int ty;
	while(x>=126 || y>=126){
		tx=0;
		if(x>110){
			tx=110;
			x-=tx;
		}

		ty=0;
		if(y>110){
			ty=110;
			y-=ty;
		}

		sendMouseData(sigX*tx, sigY*ty, 0, 0);
	}

	if(x!=0 || y!=0){
		sendMouseData(sigX*x, sigY*y, 0, 0);
	}
}

//--------------------------------------------------------------------

void TinyusbMouse::buttonDown(MOUSE_BUTTONS btn)
{
	m_button=getMouseButton(btn);
	sendMouseData();
}

//--------------------------------------------------------------------

void TinyusbMouse::buttonUp(MOUSE_BUTTONS btn)
{
	m_button=0;
	sendMouseData();
}

//--------------------------------------------------------------------
