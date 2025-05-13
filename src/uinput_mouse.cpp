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
#include "uinput_mouse.h"

#include "utilities.h"
#include "debug_utils.h"

#include <thread>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/uinput.h>

#include <wx/display.h>

#define MOUSE_NAME "kmRecorderPlayer mouse"

//====================================================================

UinputMouse::UinputMouse()
:m_fd(-1)
, c_displayWidth(wxDisplay().GetGeometry().GetWidth())
, c_displayHeight(wxDisplay().GetGeometry().GetHeight())
{
	reload();
}

//--------------------------------------------------------------------

UinputMouse::~UinputMouse()
{
	ioctl(m_fd, UI_DEV_DESTROY);
	close(m_fd);
}

//--------------------------------------------------------------------

bool UinputMouse::reload()
{
	if(m_fd<0){
		m_fd=open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_CLOEXEC);

		setLastError([this](){
			return m_fd < 1;
		});

		if(m_fd>-1){
			ioctl(m_fd, UI_SET_EVBIT, EV_KEY);
			ioctl(m_fd, UI_SET_KEYBIT, BTN_LEFT);
			ioctl(m_fd, UI_SET_KEYBIT, BTN_RIGHT);

			ioctl(m_fd, UI_SET_EVBIT, EV_ABS);
			ioctl(m_fd, UI_SET_ABSBIT, ABS_X);
			ioctl(m_fd, UI_SET_ABSBIT, ABS_Y);

			uinput_abs_setup abs_X;
			abs_X.code=0;
			abs_X.absinfo.value=0;
			abs_X.absinfo.minimum=0;
			abs_X.absinfo.maximum=c_displayWidth;
			abs_X.absinfo.fuzz=0;
			abs_X.absinfo.flat=0;
			abs_X.absinfo.resolution=0;

			ioctl(m_fd, UI_ABS_SETUP, &abs_X);

			uinput_abs_setup abs_Y;
			abs_Y.code=1;
			abs_Y.absinfo.value=0;
			abs_Y.absinfo.minimum=0;
			abs_Y.absinfo.maximum=c_displayHeight;
			abs_Y.absinfo.fuzz=0;
			abs_Y.absinfo.flat=0;
			abs_Y.absinfo.resolution=0;

			ioctl(m_fd, UI_ABS_SETUP, &abs_Y);

			init(MOUSE_NAME);
		}
	}
	return m_fd>-1;
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

void UinputMouse::setPosition(const int absX, const int absY)
{
	emit(EV_ABS, ABS_X, absX);
	emit(EV_ABS, ABS_Y, absY);
	emit(EV_SYN, SYN_REPORT, 0);
	std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

//--------------------------------------------------------------------
