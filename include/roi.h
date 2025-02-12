/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* struct ROI                                                         *
* class ROI_DC                                                       *
* class RectDC                                                       *
* class RectDC2                                                      *
* class LineDC                                                       *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _ROI_H
#define _ROI_H

#include <wx/dc.h>

extern wxColour s_colour;
extern wxBrush s_brush;

//====================================================================

struct ROI
{
	ROI()
	{
		reset();
	}

	virtual ~ROI()=default;

	void reset()
	{
		m_posX=0;
		m_posY=0;
		m_width=0;
		m_height=0;
	}

	void setCornerA(const wxPoint& pos)
	{
		m_posX=pos.x;
		m_posY=pos.y;
		m_width=0;
		m_height=0;
	}

	void setCornerB(const wxPoint& pos)
	{
		m_width=pos.x-m_posX;
		m_height=pos.y-m_posY;
	}

	int m_posX;
	int m_posY;
	int m_width;
	int m_height;
};

//====================================================================

class ROI_DC : public ROI
{
	public:
		virtual ~ROI_DC()=default;

		virtual void getValues(int& x, int& y, int& w, int& h)=0;

		virtual void draw(wxDC&  dc)=0;
};

//====================================================================

class RectDC : public ROI_DC
{
	public:
		RectDC(const wxBrush &brush)
		:m_brush(brush)
		{}

		RectDC(const wxColour &colour, wxBrushStyle brushStyle=wxBRUSHSTYLE_CROSSDIAG_HATCH)
		:m_brush(colour, brushStyle)
		{}

		RectDC()
		:m_brush(*wxBLUE, wxBRUSHSTYLE_CROSSDIAG_HATCH)
		{}

		virtual ~RectDC()=default;

		virtual void draw(wxDC&  dc)
		{
			dc.SetBrush(s_brush);
			dc.DrawRectangle(m_posX, m_posY, m_width, m_height);
		}

		virtual void getValues(int& x, int& y, int& w, int& h)
		{
			x=m_width<0? m_posX+m_width : m_posX;
			y=m_height<0? m_posY+m_height : m_posY;
			w=std::abs(m_width);
			h=std::abs(m_height);
		}

	protected:
		wxBrush m_brush;
};

//====================================================================

class RectDC2 : public RectDC
{
	public:
		RectDC2()
		:RectDC(*wxTRANSPARENT_BRUSH)
		{}

		virtual ~RectDC2()=default;

		virtual void draw(wxDC&  dc)
		{
			dc.SetBrush(m_brush);
			dc.SetPen(wxPen(wxColor(0, 255, 0), 10));
			dc.DrawRectangle(m_posX, m_posY, m_width, m_height);
		}
};

//====================================================================

class LineDC : public ROI_DC
{
	public:
		LineDC()
		{}

		virtual ~LineDC()=default;

		virtual void draw(wxDC&  dc)
		{
			dc.SetPen(wxPen(wxColor(255, 5, 0), 5)); 
			dc.DrawLine(m_posX, m_posY, m_posX+m_width, m_posY+m_height );
		}

		virtual void getValues(int& x, int& y, int& w, int& h)
		{
			x=m_posX;
			y=m_posY;
			w=m_width;
			h=m_height;
		}
};

//====================================================================

#endif
