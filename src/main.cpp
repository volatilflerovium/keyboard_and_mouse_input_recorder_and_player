/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include <csignal>
#include "main.h"
#include "km_recorder_player.h"

sig_atomic_t signal_caught = 0;

static RecorderPlayerKM* appGUI=nullptr;

void sigint_handler(int sig)
{	
	if(appGUI){
		wxDELETE(appGUI);
	}
	exit(sig);
}

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	RecorderPlayerKM* recorderPlayerGUI = new RecorderPlayerKM(wxT("kmRecorderAndPlayer"));

	recorderPlayerGUI->Show(true);

	appGUI=recorderPlayerGUI;

	signal(SIGINT, &sigint_handler);
		
	return true;
}

void MyApp::OnClose(wxCloseEvent &event)
{
	//std::cout<<"closing app!\n";
}
