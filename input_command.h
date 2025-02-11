/*********************************************************************
* class ExitCode                                                     *
* struct MouseCmdExitPosition                                        *
* class WindowOffset                                                 *
* class BaseCommand                                                  *
* class InputCommand                                                 *
* class TextCommand                                                  *
* class LineCommand                                                  *
* class KeyCommad                                                    *
* class UnicodeCommand                                               *
* class ShortcutCommand                                              *
* class MoveMouseCommand                                             *
* class MouseLeftBtnCommand                                          *
* class MouseRightBtnCommand                                         *
* class MouseSelectCommand                                           *
* class MouseSelectCommand2                                          *
* class CtrlCommand                                                  *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef INPUT_COMMAND_H
#define INPUT_COMMAND_H

#include "keyboard_emulator.h"
#include "mouse_emulator.h"

#include "utilities.h"
#include "debug_utils.h"

#include <iostream>
#include <fstream>

//====================================================================

typedef std::function<void()> Cmd;

typedef void(KeyboardEmulatorI::* Func)(void);

extern MouseEmulatorI* s_MouseEmulator;

extern KeyboardEmulatorI* s_KeyboardEmulator;

void MouseLeftClick(int x, int y);

//====================================================================

enum class CommandTypes
{
	Ctrl,
	Keyboard,
	KeyboardLine,
	KeyboardText,
	MouseMove,
	MouseLeftBtn,
	MouseRightBtn,
	MouseSelection,
	Screenshot,// alias for Ctrl
	Unicode,
	Shortcut,
};

enum class CommandInputTypes
{
	CTRL,
	INPUT,
};

enum CTRL_CMD_MODE
{
	SIMILAR=0,
	DIFFERENT
};

class ExitCode
{
	public:
		enum 
		{
			OK=0,
			FAILED=1<<1,
			TIMEOUT=1<<2,
			BASE_IMAGE_MISSING=1<<3,
			TARGET_WINDOW_CLOSED=1<<4,
			CV_EXCEPTION=1<<5,
			OUT_OF_BOUND=1<<6,// when pointer is trying to get to a position outside of the screen
			SYSTEM_FAILED=1<<7,
			UNKNOWN=1<<8,
			LAST=1<<9
		};

		static const char* getExitCodeMsg(int exitCode)
		{
			int n=ln(exitCode);
			return ExitCodeVerbose[n];
		}

	private:
		static constexpr int TOTAL_MSG=9;
		static const char* const ExitCodeVerbose[TOTAL_MSG];

		static int ln(int x);
};

//====================================================================

struct MouseCmdExitPosition
{
	static int s_x;
	static int s_y;
	static void setExitPosition();
};

//====================================================================

class WindowOffset
{
	public:
		WindowOffset()
		{}

		WindowOffset(const char* windowName)
		:m_windowName(windowName)
		{}

		virtual ~WindowOffset()=default;

		bool isAbsolute() const;
		bool windowExists();
		std::pair<int, int> getOffset(int x, int y);
		bool isTargetValid(int x, int y);
		const char* getWindowName() const;

	protected:
		std::string m_windowName;
		int m_absoluteX;
		int m_absoluteY;
};

//--------------------------------------------------------------------

inline bool WindowOffset::isAbsolute() const
{
	return m_windowName=="root" || m_windowName.length()==0;
}

//--------------------------------------------------------------------

inline bool WindowOffset::windowExists()
{
	if(isAbsolute()){
		return true;
	}
	return ::windowExists(m_windowName.c_str());
}

//--------------------------------------------------------------------

inline std::pair<int, int> WindowOffset::getOffset(int x, int y)
{
	if(isAbsolute()){
		return {x, y};
	}
	return getWindowCoord(m_windowName.c_str(), x, y);
}

//--------------------------------------------------------------------

inline const char* WindowOffset::getWindowName() const
{
	return m_windowName.c_str();
}

//====================================================================
//====================================================================

class BaseCommand
{
	public:
		BaseCommand(const char* description)
		:m_description(description)
		, m_run(true)
		{}

		virtual ~BaseCommand()=default;

		virtual void execute();
		virtual std::string toString();

		// wait after execution
		virtual uint wait() const=0;
		virtual void updateTime(uint t)=0;
		virtual bool ready()=0;
		virtual int getExitCode() const=0;

		virtual bool isActive() const;
		virtual void updateActive(bool run);
		virtual void updateDescription(const char* description);
		const char* getDescription() const;
		
		virtual void print(std::ostream& outputStream)=0;

		virtual CommandInputTypes getCmdType()=0;

	protected:
		std::string m_description;
		Cmd m_cmd{[](){}};
		StrCmd m_strCmd{[](){return "not set.";}};
		bool m_run;
};

//--------------------------------------------------------------------

inline void BaseCommand::execute()
{
	m_cmd();
}

//--------------------------------------------------------------------

inline std::string BaseCommand::toString()
{
	return m_strCmd();
}

//--------------------------------------------------------------------

inline bool BaseCommand::isActive() const
{
	return m_run;
}

//--------------------------------------------------------------------

inline void BaseCommand::updateActive(bool run)
{
	m_run=run;
}

//--------------------------------------------------------------------

inline void BaseCommand::updateDescription(const char* description)
{
	m_description=description;
}

//--------------------------------------------------------------------

inline const char* BaseCommand::getDescription() const
{
	return m_description.c_str();
}

//====================================================================
//====================================================================

class InputCommand : public BaseCommand
{
	public:
		InputCommand(const char* description, int wait)
		:BaseCommand(description)
		, m_wait(wait)
		{}

		virtual ~InputCommand()=default;

		virtual bool ready() override
		{
			return true;
		}

		virtual int getExitCode() const override
		{
			return 0;
		}

		// wait after execution
		virtual uint wait() const
		{
			return m_wait;
		}

		virtual void updateTime(uint t) override
		{
			m_wait=t;
		}

		virtual void print(std::ostream& outputStream) override
		{
			outputStream<<ToString2(m_strCmd(), m_wait);
			outputStream<<"\n";
		}

		virtual CommandInputTypes getCmdType() override
		{
			return CommandInputTypes::INPUT;
		}

	protected:
		uint m_wait;
};

//====================================================================

class TextCommand : public InputCommand
{
	public:
		TextCommand(const char* description, int wait, const std::string& text);

		virtual ~TextCommand()=default;

		static InputCommand* Builder(const char* description, int wait, const std::string& text)
		{
			return new TextCommand(description, wait, text);
		}

	private:
		std::string m_text;
};

//====================================================================

class LineCommand : public InputCommand
{
	public:
		LineCommand(const char* description, int wait, const std::string& line);

		virtual ~LineCommand()=default;

		static InputCommand* Builder(const char* description, int wait, const std::string& line)
		{
			return new LineCommand(description, wait, line);
		}

	private:
		std::string m_line;
};

//====================================================================

class KeyCommad : public InputCommand
{
	public:
		KeyCommad(const char* description, int wait, SPKEYS keyCode);

		KeyCommad(const char* description, int wait, int keyCode)
		:KeyCommad(description, wait, SPKEYS(keyCode))
		{}

		virtual ~KeyCommad()=default;

		static InputCommand* Builder(const char* description, int keyCode, int wait)
		{
			return new KeyCommad(description, wait, keyCode);
		}

	private:
		SPKEYS m_keyCode;
};

//====================================================================

class UnicodeCommand : public InputCommand
{
	public:
		UnicodeCommand(const char* description, int wait, const std::string& codePoint);

		virtual ~UnicodeCommand()=default;

		static UnicodeCommand* Builder(const char* description, int wait, const char* codePoint)
		{
			return new UnicodeCommand(description, wait, codePoint);
		}

	private:
		std::string m_codePoint;
};

//====================================================================

class ShortcutCommand : public InputCommand
{
	public:
		ShortcutCommand(const char* description, int wait, const std::string& shortcut);

		virtual ~ShortcutCommand()=default;

		static ShortcutCommand* Builder(const char* description, int wait, const char* shortcut);

	private:
		std::string m_shortcut;
};

//====================================================================

class MoveMouseCommand : public InputCommand, public WindowOffset
{
	public:
		MoveMouseCommand(const char* description, int wait, int x, int y, const char* windowName);

		virtual ~MoveMouseCommand()=default;

		static InputCommand* Builder(const char* description, int wait, int x, int y, const char* windowName)
		{
			return new MoveMouseCommand(description, wait, x, y, windowName);
		}

		virtual int getExitCode() const override
		{
			return m_statusCode;
		}

	private:
		int m_x;
		int m_y;
		uint m_statusCode;
};

//====================================================================

class MouseLeftBtnCommand : public InputCommand, public WindowOffset
{
	public:
		MouseLeftBtnCommand(const char* description, int wait, int x, int y, const char* windowName);

		virtual ~MouseLeftBtnCommand()=default;

		static InputCommand* Builder(const char* description, int wait, int x, int y, const char* windowName)
		{
			return new MouseLeftBtnCommand(description, wait, x, y, windowName);
		}

		virtual int getExitCode() const override
		{
			return m_statusCode;
		}

	private:
		int m_x;
		int m_y;
		uint m_statusCode;
};

//====================================================================

class MouseRightBtnCommand : public InputCommand, public WindowOffset
{
	public:
		MouseRightBtnCommand(const char* description, int wait, int x, int y, const char* windowName);

		virtual ~MouseRightBtnCommand()=default;

		static InputCommand* Builder(const char* description, int wait, int x, int y, const char* windowName)
		{
			return new MouseRightBtnCommand(description, wait, x, y, windowName);
		}

		virtual int getExitCode() const override
		{
			return m_statusCode;
		}

	private:
		int m_x;
		int m_y;
		uint m_statusCode;
};

//====================================================================

class MouseSelectCommand : public InputCommand, public WindowOffset
{
	public:
		MouseSelectCommand(const char* description, int wait, int posX, int posY, int width, int height, const char* windowName);

		virtual ~MouseSelectCommand()=default;

		static InputCommand* Builder(const char* description, int wait, int posX, int posY, int width, int height, const char* windowName)
		{
			return new MouseSelectCommand(description, wait, posX, posY, width, height, windowName);
		}

	private:
		int m_posX;
		int m_posY;
		int m_width;
		int m_height;
		uint m_statusCode;
};


class MouseSelectCommand2 : public InputCommand, public WindowOffset
{
	public:
		MouseSelectCommand2(const char* description, int wait, int endX, int endY, const char* windowName);

		virtual ~MouseSelectCommand2()=default;

		static InputCommand* Builder(const char* description, int wait, int endX, int endY, const char* windowName)
		{
			return new MouseSelectCommand2(description, wait, endX, endY, windowName);
		}

	private:
		int m_endX; //relative to the current window
		int m_endY;
		uint m_statusCode;
};

//====================================================================

class CtrlCommand : public BaseCommand, public WindowOffset
{
	typedef std::function<bool()> CKR;

	public:
		enum {
			WAIT=500
		};

	public:
		CtrlCommand(const char* description, const std::string& baseImageName, const char* roiStr, const char* windowName, bool removeImg=true);

		virtual ~CtrlCommand();

		static CtrlCommand* Builder(const char* description, const std::string& baseImageName, const char* roiStr, const char* windowName, uint threshold, uint sensitivity)
		{
			CtrlCommand* cmd=new CtrlCommand(description, baseImageName, roiStr, windowName);
			cmd->setThreshold(threshold);
			cmd->setSensitivity(sensitivity);
			return cmd;
		}

		virtual bool ready() override;

		virtual int getExitCode() const override
		{
			return m_statusCode | 1 * static_cast<int>(m_strictRun && m_statusCode>0);
		}

		virtual uint wait() const override
		{
			return WAIT;
		}

		virtual void updateTime(uint secs) override
		{
			m_tries=1;
			if(secs>0){
				m_tries+=(secs*2*WAIT-1)/wait();// check at least once
			}
		}

		virtual uint getTimeout() const
		{
			return m_tries/2; // every try is every 500ms, timeout is is seconds
		}

		virtual CTRL_CMD_MODE getSimilarity() const
		{
			if(m_similarity){
				return CTRL_CMD_MODE::SIMILAR;
			}
			return CTRL_CMD_MODE::DIFFERENT;
		}

		virtual void setSimilarity(bool sim)
		{
			m_similarity=sim;
		}

		virtual void print(std::ostream& outputStream) override
		{
			m_cleanImg=false;
			outputStream<<ToString2(
				static_cast<int>(CommandTypes::Ctrl),
				m_description,
				m_run,
				m_baseImageName,
				m_roiStr,
				m_windowName,
				m_similarity,
				m_threshold,
				m_sensitivity,
				m_strictRun,
				getTimeout()
			);
			outputStream<<"\n";
		}

		virtual CommandInputTypes getCmdType() override
		{
			return CommandInputTypes::CTRL;
		}

		virtual const std::string& getBaseImg() const
		{
			return m_baseImageName;
		}

		virtual void setCtrlCallback();

		virtual void updateBaseImg(const char* baseImg, const char* roiStr);

		virtual void setSensitivity(uint sensitivity)
		{
			m_sensitivity=sensitivity;
		}

		virtual uint getSensitivity() const
		{
			return m_sensitivity;
		}

		virtual uint getThreshold() const
		{
			return m_threshold;
		}

		virtual void setThreshold(uint threshold)
		{
			m_threshold=threshold;
		}

		virtual bool getRestriction() const
		{
			return m_strictRun;
		}

		virtual void setRestriction(bool restriction)
		{
			m_strictRun=restriction;
		}

	protected:
		std::string m_baseImageName;
		std::string m_roiStr;
		CKR m_cbk;
		uint m_tries;
		uint m_triesCount;
		uint m_threshold;
		uint m_statusCode;
		uint m_sensitivity;
		bool m_similarity;
		bool m_strictRun;
		bool m_cleanImg;

		void removeImg();
};

//====================================================================

#endif
