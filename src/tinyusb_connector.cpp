/*********************************************************************
* struct TinyusbConnector                                            *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "tinyusb_connector.h"

#include "debug_utils.h"

#include "TinyUSB_Link_Lib/serial_port.h"
#include "TinyUSB_Link_Lib/udp_client.h"

//====================================================================

void TinyusbConnector::setConnector(bool connectionType, const char* alpha, uint number)
{
	s_connector=NullConnector::getConnector();
	if(connectionType){
		s_connector=SerialPort::getConnector(alpha, number);
	}
	else{
		s_connector=UDPClient::getConnector(alpha, number);
	}
}

//--------------------------------------------------------------------

bool TinyusbConnector::doHandshake()
{
	if(s_connector!=NullConnector::getConnector()){
		uint8_t msg[]={0xE8, 0xE9};
		s_connector->send(msg, 2);

		for(int i=0; i<50; i++){
			if(s_connector->receive(100)){
				return true;
			}
		}
		s_connector->send(msg, 2);
		return s_connector->receive(2000);
	}
	return false;
};

//====================================================================
