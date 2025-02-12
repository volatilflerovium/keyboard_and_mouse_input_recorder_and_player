/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* template<typename, bool> class BaseWrapperPanel                    *
* class WrapperPanel : public BaseWrapperPanel<WXPANEL>              *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _WRAPPER_PANEL_H
#define _WRAPPER_PANEL_H

#include "enumerations.h"
#include "debug_utils.h"

#include <wx/panel.h>
#include <wx/menu.h>

//====================================================================

template<typename W, bool= std::is_base_of<wxPanel, W>::value>
class BaseWrapperPanel
{};

template<typename W>
class BaseWrapperPanel<W, true> : public W
{
	public:
		BaseWrapperPanel(wxWindow* parent, uint posY, uint width)
		{
			create(this, parent, posY, width);
		}

		virtual ~BaseWrapperPanel()=default;

		int getHeight() const
		{
			return m_height;
		}

		virtual bool isSelected() const=0;

	protected:	
		int m_height{0};

		void create(wxPanel* w, wxWindow* parent, uint posY, uint width)
		{
			w->Create(parent, wxID_ANY, wxPoint(0, posY), wxSize(width, 10));
		}
};

//====================================================================
//====================================================================

template<typename T, int M, int N, int WX_ID, typename WXPANEL=wxPanel>
class WrapperPanel : public BaseWrapperPanel<WXPANEL>
{
	public:
		WrapperPanel(wxWindow* parent, uint posY, uint width);

		virtual ~WrapperPanel()=default;

		virtual bool isSelected() const
		{
			return m_isSelected;
		}

	protected:
		static constexpr int _TOP_MARGIN_PADDING=M;
		static constexpr int _MARGIN_WIDTH=N;
		static T* s_lastSelected;
		wxPanel* m_handlerPtr;
		bool m_isSelected{false};

		void setBackgroundColour(wxPanel* panel, const wxColour& colour)
		{
			panel->SetBackgroundColour(colour);
		}

		void popupMenu(wxPanel* panel, wxMenu* menu)
		{
			panel->PopupMenu(menu);
		}

		virtual void setSelected()=0;
		void highlight(bool select=false);

		void MouseLeftBtnDown(wxMouseEvent& event);
		void OnContextMenu(wxContextMenuEvent& event);
};

//====================================================================

template<typename T, int M, int N, int WX_ID, typename WXPANEL>
T* WrapperPanel<T, M, N, WX_ID, WXPANEL>::s_lastSelected=nullptr;

template<typename T, int M, int N, int WX_ID, typename WXPANEL>
WrapperPanel<T, M, N, WX_ID, WXPANEL>::WrapperPanel(wxWindow* parent, uint posY, uint width)
:BaseWrapperPanel<WXPANEL>(parent, posY, width)
{
	setBackgroundColour(this, wxColour("#FFFFFF"));

	m_handlerPtr = new wxPanel(this,
		wxID_ANY,
		wxPoint(_MARGIN_WIDTH, _TOP_MARGIN_PADDING),
		wxSize(width-2*_MARGIN_WIDTH, -1)	
	);
	m_handlerPtr->SetMinSize(wxSize(width-2*_MARGIN_WIDTH, -1));

	m_handlerPtr->SetBackgroundColour(wxColour("#FFFFFF"));
}

//--------------------------------------------------------------------

template<typename T, int M, int N, int WX_ID, typename WXPANEL>
void WrapperPanel<T, M, N, WX_ID, WXPANEL>::highlight(bool select)
{
	if(m_isSelected || select){
		m_isSelected=false;
		setBackgroundColour(this, wxColour("#FFFFFF"));
	}
	else{
		m_isSelected=true;
		setBackgroundColour(this, wxColour("#8b88f1"));
	}
}

//--------------------------------------------------------------------

template<typename T, int M, int N, int WX_ID, typename WXPANEL>
void WrapperPanel<T, M, N, WX_ID, WXPANEL>::MouseLeftBtnDown(wxMouseEvent& event)
{
	if(s_lastSelected && s_lastSelected!=this){
		s_lastSelected->highlight(true);
	}

	setSelected();

	highlight();
}

//--------------------------------------------------------------------

template<typename T, int M, int N, int WX_ID, typename WXPANEL>
void WrapperPanel<T, M, N, WX_ID, WXPANEL>::OnContextMenu(wxContextMenuEvent& event)
{
	if(m_isSelected){
		wxMenu menu;
		menu.Append(WX_ID, wxT("Delete"));
		popupMenu(this, &menu);
	}
}

//====================================================================

#endif
