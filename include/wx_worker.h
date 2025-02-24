/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class WxWorker                                                     *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    24-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef WX_WORKER_H
#define WX_WORKER_H

#include "event_definitions.h"

#include <wx/wx.h>
#include <wx/thread.h>

//######################################################################

class WxWorker : public wxThread
{
	public:
		WxWorker(wxWindow* parent);

		virtual ~WxWorker()=default;

		bool isRunning();

	protected:
		virtual ExitCode Entry();

	private:
		wxWindow* m_parent;

		void fireEvent(EvtID eventID, const wxString& msg="");
};

//--------------------------------------------------------------------

// Notice that detached wxThreads delete themselves
inline WxWorker::WxWorker(wxWindow* parent)
:wxThread(wxTHREAD_DETACHED)
, m_parent(parent)
{
}

//--------------------------------------------------------------------

inline void WxWorker::fireEvent(EvtID eventID, const wxString& msg)
{
	wxCommandEvent event(wxEVT_CUSTOM_EVENT, eventID);
	event.SetString(msg);
	wxPostEvent(m_parent, event);
}

//--------------------------------------------------------------------

#endif
