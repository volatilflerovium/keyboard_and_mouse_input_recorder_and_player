/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class wxBackgroundBitmap                                           *
* class BaseBackground                                               *
* class ImagePanel                                                   *
* class ROISelectionI                                                *
* class ImageROI<W, true>                                            *
* class ImagePanelROI                                                *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef IMAGE_PANEL_H
#define IMAGE_PANEL_H

#include "event_definitions.h"
#include "roi.h"
#include "debug_utils.h"

#include <wx/wx.h>

#define DEFAULT_TRANSPARENCY 50

//====================================================================

class wxBackgroundBitmap : public wxEvtHandler
{
	public:
		wxBackgroundBitmap()
		:m_bitmap(new wxBitmap)
		{}

		wxBackgroundBitmap(wxBitmap* bitmap)
		:m_bitmap(bitmap)
		{}
		
		/*
		 * From wxBitmap documentationIf the application omits to delete 
		 * the bitmap explicitly, the bitmap will be destroyed automatically 
		 * by wxWidgets when the application exits.
		 * */
		virtual ~wxBackgroundBitmap()=default;
		virtual bool ProcessEvent(wxEvent& Event);

		void loadImage(wxBitmap* bitmap);
		void loadImage(const char* imagePath, wxBitmapType bitmapType, uint rWidth, uint rHeight);
		void loadImage(const char* imagePath, wxBitmapType bitmapType);
		int getWidth() const;
		int getHeight() const;

	protected:
		wxBitmap* m_bitmap;
};

//--------------------------------------------------------------------

inline void wxBackgroundBitmap::loadImage(wxBitmap* bitmap)
{
	m_bitmap=bitmap;
}

//--------------------------------------------------------------------

inline void wxBackgroundBitmap::loadImage(const char* imagePath, wxBitmapType bitmapType)
{
	m_bitmap->LoadFile(imagePath, bitmapType);
}

//--------------------------------------------------------------------

inline int wxBackgroundBitmap::getWidth() const
{
	return m_bitmap->GetWidth();
}

//--------------------------------------------------------------------

inline int wxBackgroundBitmap::getHeight() const
{
	return m_bitmap->GetHeight();
}

//====================================================================
//====================================================================

template<typename W, bool= std::is_base_of<wxWindow, W>::value>
class BaseBackground
{};

template<typename W>
class BaseBackground<W, true> : public W
{
	public:
		BaseBackground()
		:m_hasBackground(false)
		{}

		virtual ~BaseBackground();
		virtual void clearBackground();
		virtual void loadBackground(const char* imagePath, wxBitmapType bitmapType);
		virtual void loadBackground(wxBitmap* bitmapPtr);

	protected:
		wxBackgroundBitmap m_background;
		bool m_hasBackground;

		void setEventHandler();

	private:
		void PopEventHandler(wxWindow* wPtr);
		void PushEventHandler(wxWindow* wPtr, wxEvtHandler* handler);
};

//--------------------------------------------------------------------

template<typename W>
BaseBackground<W, true>::~BaseBackground()
{
	if(m_hasBackground){
		PopEventHandler(this);
	}
}

//--------------------------------------------------------------------

template<typename W>
void BaseBackground<W, true>::clearBackground()
{
	if(m_hasBackground){
		PopEventHandler(this);
		m_hasBackground=false;
		this->Refresh();
		this->Update();
	}
}

//--------------------------------------------------------------------

template<typename W>
void BaseBackground<W, true>::loadBackground(const char* imagePath, wxBitmapType bitmapType)
{
	m_background.loadImage(imagePath, bitmapType);
	setEventHandler();
	this->Refresh();
	this->Update();
}

//--------------------------------------------------------------------

template<typename W>
void BaseBackground<W, true>::loadBackground(wxBitmap* bitmapPtr)
{
	m_background.loadImage(bitmapPtr);
	setEventHandler();
	this->Refresh();
	this->Update();
}

//--------------------------------------------------------------------

template<typename W>
void BaseBackground<W, true>::setEventHandler()
{
	if(!m_hasBackground){
		m_hasBackground=true;	
		PushEventHandler(this, &m_background);
	}
}

//--------------------------------------------------------------------

template<typename W>
void BaseBackground<W, true>::PopEventHandler(wxWindow* wPtr)
{
	wPtr->PopEventHandler();
}

//--------------------------------------------------------------------

template<typename W>
void BaseBackground<W, true>::PushEventHandler(wxWindow* wPtr, wxEvtHandler* handler)
{
	wPtr->PushEventHandler(handler);
}

//====================================================================
//====================================================================

class ImagePanel : public BaseBackground<wxPanel>
{
	public:
		template<typename... Args>
		ImagePanel(wxWindow* parent, Args... args)
		{
			Create(parent, args...);
			m_maxWidth=GetSize().GetWidth();
			m_maxHeight=GetSize().GetHeight();
		}

		virtual ~ImagePanel()=default;

		virtual void loadBackground(const char* imagePath, wxBitmapType bitmapType=wxBITMAP_TYPE_PNG);

	protected:
		wxString m_path;
		uint m_maxWidth;
		uint m_maxHeight;

		void OnDoubleClick(wxMouseEvent& event);

		DECLARE_EVENT_TABLE()
};

//--------------------------------------------------------------------

inline void ImagePanel::OnDoubleClick(wxMouseEvent& event)
{
	std::string str("xdg-open ");
	str.append(m_path.mb_str());
	system(str.c_str());
}

//====================================================================
//====================================================================

class ROISelectionI
{
	public:
		virtual ~ROISelectionI()=default;

		virtual bool isRectangle() const=0;
		virtual void getValues(int& x, int& y, int& w, int& h)=0;
		virtual void drawLine()=0;
		virtual void drawRectangle()=0;
		virtual void clearSelection()=0;

};

//====================================================================

template<typename W, bool= std::is_base_of<wxWindow, W>::value>
class ImageROI
{};

template<typename W>
class ImageROI<W, true> : public BaseBackground<W>, public ROISelectionI
{
	public:
		ImageROI()
		: m_onContextMenu(nullptr)
		, m_roi(nullptr)
		{
		}

		virtual ~ImageROI();
		void setContextMenuEventHandler(std::function<void(wxContextMenuEvent&, wxPoint)> cbk);
		virtual void getValues(int& x, int& y, int& w, int& h) override;
		virtual void drawLine() override;
		virtual void drawRectangle() override;
		virtual void clearSelection() override;

	private:
		
		std::function<void(wxContextMenuEvent&, wxPoint)> m_onContextMenu;
		void refresh(wxWindow* w);

	protected:

		enum SELECTION
		{
			ALLOW,
			START,
			END,
		};
	
		ROI_DC* m_roi;
		SELECTION m_select;

		virtual void paintEvent(wxPaintEvent & evt);
		virtual void paintNow();

		virtual wxPoint getMousePosition()=0;
		virtual void OnContextMenu(wxContextMenuEvent& event);

		void MouseRightBtnDown(wxMouseEvent& event);
		void MouseLeftBtnDown(wxMouseEvent& event);
		void MouseLeftBtnUp(wxMouseEvent& event);
		virtual void OnMouseMove(wxMouseEvent& event);
};

//--------------------------------------------------------------------

template<typename W>
ImageROI<W, true>::~ImageROI()
{
	if(m_roi){
		m_roi->reset();
		delete m_roi;
		m_roi=nullptr;
	}
}

template<typename W>
inline void ImageROI<W, true>::setContextMenuEventHandler(std::function<void(wxContextMenuEvent&, wxPoint)> cbk)
{
	m_onContextMenu=cbk;
}

template<typename W>
void ImageROI<W, true>::getValues(int& x, int& y, int& w, int& h)
{
	m_roi->getValues(x, y, w, h);
}

//--------------------------------------------------------------------

template<typename W>
void ImageROI<W, true>::clearSelection()
{
	m_select=SELECTION::END;
	if(m_roi){
		m_roi->reset();
		delete m_roi;
		m_roi=nullptr;
	}
	refresh(this);
}
//--------------------------------------------------------------------

template<typename W>
inline void ImageROI<W, true>::paintEvent(wxPaintEvent& evt)
{
	if(m_roi){
		wxPaintDC dc(this);
		m_roi->draw(dc);
	}
}

//--------------------------------------------------------------------

template<typename W>
inline void ImageROI<W, true>::paintNow()
{
	if(m_roi){
		wxClientDC dc(this);
		m_roi->draw(dc);
	}
}

//--------------------------------------------------------------------

template<typename W>
inline void ImageROI<W, true>::drawLine()
{
	clearSelection();
	m_roi=new LineDC;
	m_select=SELECTION::ALLOW;
}

//--------------------------------------------------------------------

template<typename W>
inline void ImageROI<W, true>::drawRectangle()
{
	clearSelection();
	m_roi=new RectDC;
	m_select=SELECTION::ALLOW;
}

//--------------------------------------------------------------------

template<typename W>
inline void ImageROI<W, true>::MouseRightBtnDown(wxMouseEvent& event)
{
	if(m_select==SELECTION::ALLOW){
		clearSelection();
	}
	event.Skip();
}

//--------------------------------------------------------------------

template<typename W>
inline void ImageROI<W, true>::MouseLeftBtnDown(wxMouseEvent& event)
{
	if(m_select==SELECTION::ALLOW && m_roi){
		m_select=SELECTION::START;
		m_roi->setCornerA(getMousePosition());
	}
}

//--------------------------------------------------------------------

template<typename W>
inline void ImageROI<W, true>::MouseLeftBtnUp(wxMouseEvent& event)
{
	if(m_select==SELECTION::START){
		m_select=SELECTION::END;
		postEvent(this, wxEVT_SELECTION_EVENT, EvtID::NEW_SELECTION);
	}
}

//--------------------------------------------------------------------

template<typename W>
inline void ImageROI<W, true>::OnMouseMove(wxMouseEvent& event)
{
	if(m_select==SELECTION::START){
		m_roi->setCornerB(getMousePosition());
		refresh(this);
		paintNow();
	}
}

//--------------------------------------------------------------------

template<typename W>
inline void ImageROI<W, true>::refresh(wxWindow* w)
{
	w->Refresh();
}

//--------------------------------------------------------------------

template<typename W>
void ImageROI<W, true>::OnContextMenu(wxContextMenuEvent& event)
{
	if(m_onContextMenu){
		m_onContextMenu(event, getMousePosition());
	}
}

//====================================================================
//====================================================================

class ImagePanelROI : public ImageROI<wxPanel>
{
	public:
		template<typename... Args>
		ImagePanelROI(wxWindow* parent, Args... args);

		virtual ~ImagePanelROI() =default;
		virtual void loadBackground(const char* imagePath, wxBitmapType bitmapType);
		virtual bool isRectangle() const override;

	private:
		virtual wxPoint getMousePosition();
		void OnMouseLeave(wxMouseEvent& event);

		DECLARE_EVENT_TABLE()
};

//--------------------------------------------------------------------

template<typename... Args>
inline ImagePanelROI::ImagePanelROI(wxWindow* parent, Args... args)
{
	m_roi=new RectDC;
	Create(parent, args...);
}

//--------------------------------------------------------------------

inline void ImagePanelROI::loadBackground(const char* imagePath, wxBitmapType bitmapType)
{
	clearBackground();
	m_background.loadImage(imagePath, bitmapType);
	SetMinSize(wxSize(m_background.getWidth(), m_background.getHeight()));
	setEventHandler();
}

//--------------------------------------------------------------------

inline bool ImagePanelROI::isRectangle() const
{
	return true;
}

//--------------------------------------------------------------------

inline wxPoint ImagePanelROI::getMousePosition()
{
	return ScreenToClient(wxGetMousePosition());
}

//--------------------------------------------------------------------

inline void ImagePanelROI::OnMouseLeave(wxMouseEvent& event)
{
	m_select=SELECTION::END;
}

//====================================================================

#endif
