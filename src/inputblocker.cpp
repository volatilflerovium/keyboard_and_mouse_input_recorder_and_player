/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class InputBloker                                                  *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "inputblocker.h"

//====================================================================

InputBloker::InputBloker(wxWindow* parent, int transparency)
:m_windowRect(nullptr)
, m_transparency(transparency)
, m_isIconized(true)
{
	m_roi=nullptr;

	Create(parent, wxID_ANY, "Input Blocker",
	wxDefaultPosition, wxDefaultSize,
	wxFRAME_NO_WINDOW_MENU);

	SetTransparent(transparency);
	blockInput(false);
}

//--------------------------------------------------------------------

BEGIN_EVENT_TABLE(InputBloker, BaseBackground<wxFrame>)

	EVT_RIGHT_DOWN(InputBloker::MouseRightBtnDown)
	EVT_LEFT_DOWN(InputBloker::MouseLeftBtnDown)
	EVT_LEFT_UP(InputBloker::MouseLeftBtnUp)
	EVT_MOTION(InputBloker::OnMouseMove)	
	EVT_PAINT(InputBloker::paintEvent)
	EVT_CONTEXT_MENU(InputBloker::OnContextMenu)

END_EVENT_TABLE()

//--------------------------------------------------------------------

void InputBloker::reset()
{
	clearBackground();
	SetTransparent(m_transparency);

	if(!m_isIconized){
		ShowFullScreen(false, wxFULLSCREEN_ALL);
		Update();
		ShowFullScreen(true, wxFULLSCREEN_ALL);
		Raise();
	}
}

//--------------------------------------------------------------------

void InputBloker::setValidRect(int x, int y, int w, int h)
{
	m_rect.x=x;
	m_rect.y=y;
	m_rect.width=w;
	m_rect.height=h;

	if(!m_windowRect){
		m_windowRect=new RectDC2;
	}
	m_windowRect->m_posX=x;
	m_windowRect->m_posY=y;
	m_windowRect->m_width=w;
	m_windowRect->m_height=h;
	Refresh();
}

//--------------------------------------------------------------------

void InputBloker::blockInput(bool block)
{
	if(!block){
		Hide();
		Refresh();
	}

	ShowFullScreen(block, wxFULLSCREEN_ALL);
	m_isIconized=!block;
	Iconize(m_isIconized);
	if(block){
		Refresh();
		Raise();
	}
}

//====================================================================
