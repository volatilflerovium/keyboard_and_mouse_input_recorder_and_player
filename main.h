#include <wx/wx.h>

class MyApp : public wxApp
{
	public:
		virtual bool OnInit();
		virtual void OnClose(wxCloseEvent &event);
};
