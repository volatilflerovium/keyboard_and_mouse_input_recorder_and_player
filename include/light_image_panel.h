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
#ifndef LIGHT_IMAGE_PANEL_H
#define LIGHT_IMAGE_PANEL_H

#include <wx/panel.h>
#include <wx/string.h>

//====================================================================

class wxBitmap;

class LightImagePanel : public wxPanel
{
	public:
		LightImagePanel(wxWindow* parent, const char* file, const wxPoint& pos, int width);

		LightImagePanel(wxWindow* parent, const wxSize& size=wxDefaultSize);

		virtual ~LightImagePanel();

		void loadImage(const wxString& file, bool nativeSize=false);
		void replaceImage(const wxString& file);
		void loadAndFit(const wxString& file, uint width, uint height);
		void reSize(uint width, uint height);
		void fitToWidth(uint newWidth);
		void fitToHeight(uint newHeight);

	private:
		wxString m_file;
		wxBitmap* m_bitmap;	
		uint m_width;
		uint m_height;

		void fitToSize(uint rWidth, uint rHeight);
		bool reloadImage();
		void paintNow();
		
		void render(wxDC& dc);
		void paintEvent(wxPaintEvent& evt);
		
		void OnDoubleClick(wxMouseEvent& event);

		DECLARE_EVENT_TABLE()	
};

//----------------------------------------------------------------------

inline void LightImagePanel::fitToWidth(uint newWidth)
{
	reSize(newWidth, 0);
}

//----------------------------------------------------------------------

inline void LightImagePanel::fitToHeight(uint newHeight)
{
	reSize(0, newHeight);
}

//--------------------------------------------------------------------

inline void LightImagePanel::OnDoubleClick(wxMouseEvent& event)
{
	std::string str("xdg-open ");
	str.append(m_file.mb_str());
	system(str.c_str());
}

//----------------------------------------------------------------------

#endif
