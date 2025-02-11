/*********************************************************************
* class UinputMouse                                                  *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "uinput_mouse.h"

#include "utilities.h"
#include "debug_utils.h"

#include <thread>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/uinput.h>

#define MOUSE_NAME "AutomaticTester mouse"


//====================================================================

UinputMouse::UinputMouse()
:m_fd(open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_CLOEXEC))
{
	setLastError([this](){
		return m_fd < 1;
	});

	if(m_fd>-1){
		ioctl(m_fd, UI_SET_EVBIT, EV_KEY);
		ioctl(m_fd, UI_SET_KEYBIT, BTN_LEFT);
		ioctl(m_fd, UI_SET_KEYBIT, BTN_RIGHT);
		ioctl(m_fd, UI_SET_EVBIT, EV_REL);
		ioctl(m_fd, UI_SET_RELBIT, REL_X);
		ioctl(m_fd, UI_SET_RELBIT, REL_Y);

		init(MOUSE_NAME);
	}
}

//--------------------------------------------------------------------

UinputMouse::~UinputMouse()
{
	ioctl(m_fd, UI_DEV_DESTROY);
	close(m_fd);
}

//--------------------------------------------------------------------

void UinputMouse::init(const char* deviceName)
{
	std::memset(&m_usetup, 0, sizeof(m_usetup));
	m_usetup.id.bustype = BUS_USB;
	m_usetup.id.vendor  = 0x1234;
	m_usetup.id.product = 0x5678;
	//m_usetup.id.version = 1;
	strcpy(m_usetup.name, deviceName);

	ioctl(m_fd, UI_DEV_SETUP, &m_usetup);
	ioctl(m_fd, UI_DEV_CREATE);
}

//--------------------------------------------------------------------

bool UinputMouse::emit(int type, int code, int val)
{
   m_inputEvent.type = type;
   m_inputEvent.code = code;
   m_inputEvent.value = val;
   // timestamp values below are ignored
   m_inputEvent.time.tv_sec = 0;
   m_inputEvent.time.tv_usec = 0;

   return 0<write(m_fd, &m_inputEvent, sizeof(m_inputEvent));
}

//--------------------------------------------------------------------

void UinputMouse::buttonDown(MOUSE_BUTTONS button)
{
	emit(EV_KEY, getMouseButton(button), 1);
	emit(EV_SYN, SYN_REPORT, 0);
}

//--------------------------------------------------------------------

void UinputMouse::buttonUp(MOUSE_BUTTONS button)
{
	emit(EV_KEY, getMouseButton(button), 0);
	emit(EV_SYN, SYN_REPORT, 0);
}

//--------------------------------------------------------------------

void UinputMouse::setPosition(const int dx, const int dy)
{
	emit(EV_REL, REL_X, dx);
	emit(EV_REL, REL_Y, dy);
	emit(EV_SYN, SYN_REPORT, 0);
	std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

//--------------------------------------------------------------------
