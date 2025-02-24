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
#ifndef _ERROR_REPORTING_H
#define _ERROR_REPORTING_H

#include <functional>

class ErrorReporting
{
	public:
		~ErrorReporting()=default;
		const char* getLastError() const;

		void clearError();

	protected:
		void setLastError(std::function<bool(void)> cbk, const char* msg="");
	
	private:
		static const int c_errorBufSize=124;
		char m_errorMsgBuf[c_errorBufSize];
		int m_lastError{0};
};

//--------------------------------------------------------------------

inline const char* ErrorReporting::getLastError() const
{
	return m_errorMsgBuf;
}

//--------------------------------------------------------------------


#endif
