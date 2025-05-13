/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class TinyusbMouse                                                 *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "tinyusb_mouse.h"
#include "debug_utils.h"

#include <cstdint>

#include <wx/display.h>

//====================================================================

TinyusbMouse::TinyusbMouse()
: c_displayWidth(wxDisplay().GetGeometry().GetWidth())
, c_displayHeight(wxDisplay().GetGeometry().GetHeight())
, m_absX(0)
, m_absY(0)
, m_button(0)
{
	setLastError([this](){
		return !s_connector->isActive();
	});
}

//--------------------------------------------------------------------

void TinyusbMouse::sendMouseData(int8_t scrollV, int8_t scrollH)
{
	MouseData data;

	data.btn=m_button;
	data.absX=(m_absX*MAX_DISPLAY_SIZE)/c_displayWidth;
	data.absY=(m_absY*MAX_DISPLAY_SIZE)/c_displayHeight;
	data.scrollV=scrollV;
	data.scrollH=scrollH;

	sendAndWait((int8_t*)&data, 8);
}

//====================================================================
