/*********************************************************************
* class TinyusbMouse                                                 *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _TINYUSB_MOUSE_H
#define _TINYUSB_MOUSE_H

#include "mouse_emulator.h"

#include "tinyusb_connector.h"

#define MOUSE_MESSAGE 0xEB
#define TU_BIT(n) (1UL << (n))

//====================================================================

class TinyusbMouse : protected TinyusbConnector, public MouseEmulatorI
{
	enum TINYUSB_MOUSE_BUTTONS
	{
	  LEFT     = TU_BIT(0), ///< Left button
	  RIGHT    = TU_BIT(1), ///< Right button
	  MIDDLE   = TU_BIT(2), ///< Middle button
	  BACKWARD = TU_BIT(3), ///< Backward button,
	  FORWARD  = TU_BIT(4), ///< Forward button,
	};

	public:
		TinyusbMouse();
		virtual ~TinyusbMouse()=default;

	private:
		uint8_t m_button;
		void sendMouseData(int8_t deltaX, int8_t deltaY, int8_t scrollV, int8_t scrollH);
		void sendMouseData();

		TINYUSB_MOUSE_BUTTONS getMouseButton(const MOUSE_BUTTONS btn);

		/*
		 * Move the mouse to the relative position (dx, dy)
		 * */
		virtual void setPosition(const int dx, const int dy) override;

		virtual void buttonDown(MOUSE_BUTTONS btn) override;
		virtual void buttonUp(MOUSE_BUTTONS btn) override;
};

//--------------------------------------------------------------------

inline TinyusbMouse::TINYUSB_MOUSE_BUTTONS TinyusbMouse::getMouseButton(const MOUSE_BUTTONS btn)
{
	if(btn==MOUSE_BUTTONS::LEFT){
		return TINYUSB_MOUSE_BUTTONS::LEFT;
	}
	if(btn==MOUSE_BUTTONS::RIGHT){
		return TINYUSB_MOUSE_BUTTONS::RIGHT;
	}
	return TINYUSB_MOUSE_BUTTONS::MIDDLE;
}

//--------------------------------------------------------------------

inline void TinyusbMouse::sendMouseData()
{
	sendMouseData(0, 0, 0, 0);
}

//--------------------------------------------------------------------

#endif
