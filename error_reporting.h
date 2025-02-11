/*********************************************************************
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

#endif
