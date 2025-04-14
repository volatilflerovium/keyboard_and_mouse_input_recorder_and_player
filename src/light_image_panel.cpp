/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* LightImagePanel class                                   				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021   (Reviewed 03/2015)                           *
* Author:  Dan Machado                                               *
* Note: partially taken from https://wiki.wxwidgets.org/An_image_panel *
**********************************************************************/
#include "light_image_panel.h"

#include <cassert>

#include <wx/wx.h>
#include <wx/image.h>

//====================================================================

LightImagePanel::LightImagePanel(wxWindow* parent, const char* file, const wxPoint& pos, int width)
: wxPanel(parent, wxID_ANY, pos)
, m_file(file)
, m_bitmap(new wxBitmap)
, m_width(width)
, m_height(0)
{
	if(reloadImage()){
		fitToSize(m_width, m_height);
	}
}


//----------------------------------------------------------------------

LightImagePanel::LightImagePanel(wxWindow* parent, const wxSize& size)
:wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
, m_bitmap(new wxBitmap)
, m_width(0)
, m_height(0)
{
	if(size!=wxDefaultSize){
		m_width=size.GetWidth();
		m_height=size.GetHeight();
		SetMinSize(FromDIP(wxSize(m_width, m_height)));
		SetSize(FromDIP(wxSize(m_width, m_height)));
	}
}

//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(LightImagePanel, wxPanel)
	EVT_PAINT(LightImagePanel::paintEvent)
	EVT_LEFT_DCLICK(LightImagePanel::OnDoubleClick)
END_EVENT_TABLE()

//----------------------------------------------------------------------

LightImagePanel::~LightImagePanel()
{
	wxDELETE(m_bitmap);
}

//----------------------------------------------------------------------

bool LightImagePanel::reloadImage()
{
	if(m_file.Length()>0){
		m_bitmap->LoadFile(m_file, wxBITMAP_TYPE_PNG);
		return true;
	}
	return false;
}

//--------------------------------------------------------------------

void LightImagePanel::replaceImage(const wxString& file)
{
	assert(m_width+m_height!=0);

	m_file=file;

	if(reloadImage()){
		fitToSize(m_width, m_height);
	}
}

//--------------------------------------------------------------------

void LightImagePanel::loadImage(const wxString& file, bool nativeSize)
{
	m_file=file;
	if(reloadImage()){
		if(nativeSize){
			m_width=m_bitmap->GetWidth();
			m_height=m_bitmap->GetHeight();
		}
		fitToSize(m_width, m_height);
	}
}

//----------------------------------------------------------------------

void LightImagePanel::loadAndFit(const wxString& file, uint width, uint height)
{
	m_file=file;
	m_width=width;
	m_height=height;
	if(reloadImage()){
		fitToSize(m_width, m_height);
	}
}

//----------------------------------------------------------------------

void LightImagePanel::reSize(uint width, uint height)
{
	if(m_width==width && m_height==height){
		return;
	}

	m_width=width;
	m_height=height;
	if(reloadImage()){
		fitToSize(m_width, m_height);
	}
}

//----------------------------------------------------------------------

void LightImagePanel::fitToSize(uint rWidth, uint rHeight)
{
	if(rWidth+rHeight==0){
		return;
	}

	int width=m_bitmap->GetWidth();
	int height=m_bitmap->GetHeight();

	float t=(1.0*rWidth)/(1.0*width);
	float r=(1.0*rHeight)/(1.0*height);

	if(t==0 || (r>0 && r<t)){
		t=r;
	}

	*m_bitmap=wxBitmap(m_bitmap->ConvertToImage().Scale(t*width, t*height, wxIMAGE_QUALITY_HIGH));

	SetMinSize(FromDIP(wxSize(t*width, t*height)));
	SetSize(FromDIP(wxSize(t*width, t*height)));
	Refresh();
}

//----------------------------------------------------------------------

void LightImagePanel::paintEvent(wxPaintEvent & evt)
{
	wxPaintDC dc(this);
	render(dc);
}

//----------------------------------------------------------------------

void LightImagePanel::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
	wxClientDC dc(this);
	render(dc);
}

//----------------------------------------------------------------------

void LightImagePanel::render(wxDC&  dc)
{
	if(m_file.Length()>0){
		dc.DrawBitmap(*m_bitmap, 0, 0, false );
	}
}
 
//----------------------------------------------------------------------
