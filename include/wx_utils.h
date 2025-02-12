/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* wxBoxSizer* BoxSizerH()                                            *
* wxBoxSizer* BoxSizerV()                                            *
* template<typename... Args> void wxMsgBox                           *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _WX_UTILS_H
#define _WX_UTILS_H

#include <wx/gdicmn.h> 
#include <wx/sizer.h>
#include <wx/string.h>

//====================================================================

inline wxBoxSizer* BoxSizerH()
{
	return new wxBoxSizer(wxHORIZONTAL);
}

inline wxBoxSizer* BoxSizerV()
{
	return new wxBoxSizer(wxVERTICAL);
}

template<typename... Args>
inline void wxMsgBox(const char* format, Args... args)
{
	wxString msg=wxString::Format(format, args...);
	wxMessageBox(msg);
};

//====================================================================

#endif
