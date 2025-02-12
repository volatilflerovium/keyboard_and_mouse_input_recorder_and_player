/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class WX_TextCtrl                                                  *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "wx_textctrl.h"

#include <wx/valnum.h>

//====================================================================

wxIntegerValidator<unsigned int> s_integerValidator;

//====================================================================

BEGIN_EVENT_TABLE(WX_TextCtrl, wxTextCtrl)
	EVT_LEFT_DOWN(WX_TextCtrl::MouseLeftBtnDown)
	EVT_RIGHT_DOWN(WX_TextCtrl::MouseRightBtnDown)
	EVT_KEY_DOWN(WX_TextCtrl::OnKeyDown)
	EVT_SET_FOCUS(WX_TextCtrl::OnFocus)
	EVT_COMMAND(EvtID::TXTCTRL_KILL_FOCUS, wxEVT_TXTCTRL_COMMAND, WX_TextCtrl::RemoveFocus)
END_EVENT_TABLE()

//--------------------------------------------------------------------

void WX_TextCtrl::inputHander()
{
	m_enable=!m_enable;
	if(!m_enable){
		Disable(); //Disabling and then enabling remove the focus
		SetBackgroundColour(wxColour("#FFFFFF"));
		if(m_cbk){
			m_cbk(this->GetValue().mb_str());
		}
		Enable();
	}
	else{
		SetBackgroundColour(wxColour("#FFFFFF"));
	}
}

//--------------------------------------------------------------------

