/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* Some custom event definitions                                      *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "event_definitions.h"

wxDEFINE_EVENT(wxEVT_CUSTOM_EVENT, wxCommandEvent);

wxDEFINE_EVENT(wxEVT_SELECTION_EVENT, wxCommandEvent);

wxDEFINE_EVENT(wxEVT_COMMAND_MYTHREAD_COMPLETED, wxThreadEvent);

wxDEFINE_EVENT(wxEVT_POPUP_DISAPPEARED, wxCommandEvent);					

wxDEFINE_EVENT(wxEVT_TXTCTRL_COMMAND, wxCommandEvent);

wxDEFINE_EVENT(wxEVT_CUSTOM_THREAD_EVENT, wxThreadEvent);
