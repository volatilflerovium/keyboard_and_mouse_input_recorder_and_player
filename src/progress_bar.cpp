/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class ProgressBar                                                  *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    22-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/

#include "progress_bar.h"
#include "debug_utils.h"

//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(ProgressBar, wxPanel)
	EVT_PAINT(ProgressBar::OnPaint)
	EVT_TIMER(WX::PROGRESS_TIMER, ProgressBar::OnTimer)
END_EVENT_TABLE()

//----------------------------------------------------------------------

void ProgressBar::OnTimer(wxTimerEvent& event)
{
	m_range=GetSize().GetWidth();
	Refresh();
	if(m_running){
		m_timer.StartOnce(20);
	}
	else{
		m_direction=0;
		m_position=0;
	}
}

//----------------------------------------------------------------------

void ProgressBar::reset()
{
	m_position=0;
	m_direction=1;
	m_range=GetSize().GetWidth();

	m_running=true;
	Refresh();
	Update();
	m_timer.StartOnce(100);
}

//----------------------------------------------------------------------

void ProgressBar::OnPaint(wxPaintEvent& event)
{
	draw();
}

void ProgressBar::draw()
{
	wxPaintDC dc(this);

	wxPen pen(wxColour("#80b3ff"));
	dc.SetPen(pen);

	dc.DrawRectangle(wxRect(0, 0, m_range, m_height));

	wxBrush brush1(wxColour("#00e600"));
	dc.SetBrush(brush1);
	dc.DrawRectangle(wxRect(m_position, 0, m_wd, m_height));
	m_position+=m_direction*5;
	if(m_position+m_wd>m_range || m_position<0){
		m_direction*=-1;
		m_position=(m_range-m_wd)*static_cast<int>(m_direction<0);
	}
}

//----------------------------------------------------------------------
