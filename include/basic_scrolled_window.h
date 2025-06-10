/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
*                                                  *
* class FileScrolledWindow        	                                 *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _BASIC_SCROLLED_WINDOW_H
#define _BASIC_SCROLLED_WINDOW_H

#include "debug_utils.h"

#include <wx/scrolwin.h>

#include <list>

class BasicScrolledWindow : public wxScrolledWindow
{
	public:
		BasicScrolledWindow(wxWindow* parent, int Id, wxPoint Point, wxSize wSize);

		virtual ~BasicScrolledWindow();

		void addPanel(wxPanel* panelPtr);

		size_t size() const;

		void clear();

	private:
		std::list<wxPanel*> m_panelList;
		
		const int m_width;
		int m_height;
		bool m_init;

		void updateScroll(int height);
};


inline BasicScrolledWindow::BasicScrolledWindow(wxWindow* parent, int Id, wxPoint Point, wxSize wSize)
:wxScrolledWindow(parent, Id, Point, wSize)
, m_width(wSize.GetWidth())
, m_height(0)
{
	SetBackgroundColour(wxColour("#FFFFFF"));
}

inline BasicScrolledWindow::~BasicScrolledWindow()
{
	for(wxPanel* panelPtr : m_panelList){
		wxDELETE(panelPtr);
	}
}

//--------------------------------------------------------------------

inline size_t BasicScrolledWindow::size() const
{
	return m_panelList.size();
}

//--------------------------------------------------------------------

inline void BasicScrolledWindow::updateScroll(int height)
{
	m_height+=height;

	const int stepY=10;
	int hy=1+std::floor(m_height/stepY);

	SetScrollbars(1, stepY, 0, hy, 0, 0, true);
	Scroll(0, hy);
}

//----------------------------------------------------------------------

inline void BasicScrolledWindow::addPanel(wxPanel* panelPtr)
{
	Scroll(0, 0);// This is needed otherwise the panel is not position correctly
	auto size=panelPtr->GetSize();
	panelPtr->SetPosition(wxPoint(0, m_height));
	updateScroll(size.GetHeight());
	m_panelList.push_back(panelPtr);
}

//--------------------------------------------------------------------

inline void BasicScrolledWindow::clear()
{
	m_height=0;
	for(wxPanel* panelPtr : m_panelList){
		wxDELETE(panelPtr);
	}

	m_panelList.clear();

	SetScrollbars(1, 1, 0, 0, 0, 0);
	Scroll(0, 0);
}

//====================================================================

#endif
