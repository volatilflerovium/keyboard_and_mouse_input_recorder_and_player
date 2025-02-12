/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class ErrorReporting                                               *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "error_reporting.h"
#include "debug_utils.h"

#include <cstring>
#include <errno.h>   // Error integer and strerror() function

//--------------------------------------------------------------------

void ErrorReporting::setLastError(std::function<bool(void)> cbk, const char* msg)
{
	m_lastError=0;
	std::memset(m_errorMsgBuf, 0, c_errorBufSize);
	if(cbk()){
		m_lastError=errno;

		if(std::strlen(msg)>0){
			std::memcpy(m_errorMsgBuf, msg, c_errorBufSize-1);
		}
		else{
			std::memcpy(m_errorMsgBuf, strerror(m_lastError), c_errorBufSize-1);
		}

		dbg("HID last error: ", m_errorMsgBuf, " : ", std::strerror(m_lastError));
	}
}

//====================================================================
