/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class WxWorker                                                     *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    24-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "wx_worker.h"

#include "hid_manager.h"

//--------------------------------------------------------------------

wxThread::ExitCode WxWorker::Entry()
{
	int errorCode=HIDManager::connectionError();

	if(errorCode>0){
		fireEvent(EvtID::CONNECTION_FAILED, HIDManager::verboseError(errorCode));
	}
	else{
		fireEvent(EvtID::CONNECTION_OK);
	}

	return 0;
}

//--------------------------------------------------------------------
