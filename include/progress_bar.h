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
#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include "enumerations.h"
#include <wx/wx.h>
#include <wx/panel.h>

//######################################################################

class ProgressBar : public wxPanel
{
	public:
		template<typename... Args>
		ProgressBar(wxWindow* parent, Args... args)
		:wxPanel(parent, args...)
		, m_timer(this, WX::PROGRESS_TIMER)
		, m_height(20)
		, m_range(0)
		, m_position(0)
		, m_wd(20)
		, m_direction(1)
		, m_running(true)
		{
			wxColour colour(wxT("#ffffff"));
			SetBackgroundColour(colour);
			//Connect(wxEVT_PAINT, wxPaintEventHandler(ProgressBar::OnPaint));
		}

		virtual ~ProgressBar()=default;
		void reset();
		void stop()
		{
			m_running=false;
		}

	private:
		wxTimer m_timer;
		const int m_height;
		int m_range;
		int m_position;
		int m_wd;
		int m_direction;
		bool m_running;

		void draw();
		void OnTimer(wxTimerEvent& event);
		void OnPaint(wxPaintEvent& event);

	DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------




#endif
