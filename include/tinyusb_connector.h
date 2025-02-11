/*********************************************************************
* struct TinyusbConnector                                            *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _TINYUSB_CONNECTOR_H
#define _TINYUSB_CONNECTOR_H

#include "TinyUSB_Link_Lib/connector_interface.h"
#include "debug_utils.h"

#include <cstring>
#include <thread>

//====================================================================

struct TinyusbConnector
{
	static ConnectorI* s_connector;

	static void setConnector(bool connectionType, const char* alpha, uint numb);

	static void sendAndWait(void* data, uint dataSize)
	{
		s_connector->send(data, dataSize);
		if(s_connector->receive(500)){
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	static bool doHandshake();
};

//====================================================================

#endif
