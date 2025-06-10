#ifndef SCREENSHOT_WORKER_H
#define SCREENSHOT_WORKER_H

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

class ScreenshotWorker
{
	public:
		ScreenshotWorker()
		://m_ulock(m_mtx)
		 m_threadPtr(nullptr)
		, m_running(false)
		, m_continue(false)
		{}

		virtual ~ScreenshotWorker();

		static ScreenshotWorker* GetWorker()
		{
			static ScreenshotWorker worker;
			return &worker;
		}

		void init();

		bool takeScreenshot();

		void setStringCommand(const char* screenshotCmdStr)
		{
			m_screenshotCmdStr=screenshotCmdStr;
		}

		bool takeScreenshot(const char* screenshotCmdStr)
		{
			setStringCommand(screenshotCmdStr);
			return takeScreenshot();
		}

	private:
		std::mutex m_mtx;
		std::unique_lock<std::mutex> m_ulock;
		std::condition_variable m_cv;
		std::string m_screenshotCmdStr;
		std::thread* m_threadPtr;
		bool m_running;
		bool m_continue;
};

//====================================================================

#endif
