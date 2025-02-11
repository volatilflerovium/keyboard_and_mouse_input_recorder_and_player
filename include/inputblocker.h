/*********************************************************************
* class InputBloker                                                  *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _INPUTBLOCKER_H
#define _INPUTBLOCKER_H

#include "image_panel.h"

//====================================================================

class InputBloker : public ImageROI<wxFrame>
{
	public:
		InputBloker(wxWindow* parent, int transparency=DEFAULT_TRANSPARENCY);

		virtual ~InputBloker()=default;

		void setValidRect(int x, int y, int w, int h);
		void clearWindowRect();
		void loadBackground(const char* imagePath);

		void blockInput(bool block);

		virtual void reset();
		virtual void setTransparency(unsigned int transparency);
		virtual unsigned int getTransparency() const;
		virtual bool isRectangle() const;


	protected:
		virtual void OnMouseMove(wxMouseEvent& event) override;
		virtual void paintNow();
		virtual void paintEvent(wxPaintEvent& evt);

	private:
		wxRect m_rect;
		RectDC* m_windowRect;
		unsigned int m_transparency;
		bool m_isIconized;

		virtual wxPoint getMousePosition() override;

		DECLARE_EVENT_TABLE()
};

//--------------------------------------------------------------------

inline wxPoint InputBloker::getMousePosition()
{
	return wxGetMousePosition();
}

//--------------------------------------------------------------------

inline void InputBloker::loadBackground(const char* imagePath)
{
	SetTransparent(255);
	m_background.loadImage(imagePath, wxBITMAP_TYPE_PNG);
	setEventHandler();
}

//--------------------------------------------------------------------

inline void InputBloker::setTransparency(unsigned int transparency)
{
	m_transparency=transparency;
	reset();
}

//--------------------------------------------------------------------

inline unsigned int InputBloker::getTransparency() const
{
	return m_transparency;
}

//--------------------------------------------------------------------

inline bool InputBloker::isRectangle() const
{
	return nullptr!=dynamic_cast<RectDC*>(m_roi);
}

//-------------------------------------------------------------------

inline void InputBloker::OnMouseMove(wxMouseEvent& event)
{
	if(m_rect.Contains(getMousePosition())){
		ImageROI<wxFrame>::OnMouseMove(event);
	}
	else{
		event.Skip();
	}
}

//--------------------------------------------------------------------

inline void InputBloker::clearWindowRect()
{
	if(m_windowRect){
		m_windowRect->reset();
		delete m_windowRect;
		m_windowRect=nullptr;
		Refresh();
	}
}

//--------------------------------------------------------------------

inline void InputBloker::paintEvent(wxPaintEvent& evt)
{
	if(m_windowRect){
		wxPaintDC dc(this);
		m_windowRect->draw(dc);
	}
	ImageROI<wxFrame>::paintEvent(evt);
}

//--------------------------------------------------------------------

inline void InputBloker::paintNow()
{
	if(m_windowRect){
		wxClientDC dc(this);
		m_windowRect->draw(dc);
	}
	ImageROI<wxFrame>::paintNow();
}

//====================================================================

#endif
