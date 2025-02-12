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
#include "image_panel.h"

//====================================================================

void wxBackgroundBitmap::loadImage(const char* imagePath, wxBitmapType bitmapType, uint rWidth, uint rHeight)
{
	loadImage(imagePath, bitmapType);

	if(rWidth*rHeight==0){
		return;
	}

	int width=m_bitmap->GetWidth();
	int height=m_bitmap->GetHeight();

	float t=(1.0*rWidth)/(1.0*width);
	float r=(1.0*rHeight)/(1.0*height);
	if(r<t){
		t=r;
	}

	width=t*width;
	height=t*height;
	//m_bitmap.SetScaleFactor(1.0/t); // quality is not good
	*m_bitmap=(m_bitmap->ConvertToImage()).Scale(width, height, wxIMAGE_QUALITY_HIGH);
}

//--------------------------------------------------------------------

bool wxBackgroundBitmap::ProcessEvent(wxEvent& Event)
{
	if(Event.GetEventType() == wxEVT_ERASE_BACKGROUND){
		wxEraseEvent& EraseEvent = dynamic_cast<wxEraseEvent&>(Event);
		wxDC* DC = EraseEvent.GetDC();
		DC->DrawBitmap(*m_bitmap, 0, 0, false);
		return true;
	}
	return wxEvtHandler::ProcessEvent(Event);
}

//====================================================================
//====================================================================

BEGIN_EVENT_TABLE(ImagePanel, wxPanel)
	EVT_LEFT_DCLICK(ImagePanel::OnDoubleClick)
END_EVENT_TABLE()

//--------------------------------------------------------------------

void ImagePanel::loadBackground(const char* imagePath, wxBitmapType bitmapType)
{
	m_path=imagePath;

	m_background.loadImage(imagePath, bitmapType, m_maxWidth, m_maxHeight);

	int width=m_background.getWidth();
	int height=m_background.getHeight();

	SetSize(width, height);
	SetMinSize(wxSize(width, height));

	setEventHandler();
}

//====================================================================
//====================================================================

BEGIN_EVENT_TABLE(ImagePanelROI, BaseBackground<wxPanel>)

	EVT_RIGHT_DOWN(ImagePanelROI::MouseRightBtnDown)
	EVT_LEFT_DOWN(ImagePanelROI::MouseLeftBtnDown)
	EVT_LEFT_UP(ImagePanelROI::MouseLeftBtnUp)
	EVT_MOTION(ImagePanelROI::OnMouseMove)
	EVT_PAINT(ImagePanelROI::paintEvent)
	EVT_CONTEXT_MENU(ImagePanelROI::OnContextMenu)

	EVT_LEAVE_WINDOW(ImagePanelROI::OnMouseLeave)

END_EVENT_TABLE()

//====================================================================
