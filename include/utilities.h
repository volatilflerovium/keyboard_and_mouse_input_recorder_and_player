/*********************************************************************
* struct WindowRect                                                  *
* WindowRect getWindowRect                                           *
* std::pair<int, int> getWindowCoord(const char*, int, int, const char*)
* std::string resourcePath(const char*);                             *
* std::string getFilePath(const char*);                              *
* std::string getImgPath(const char*);                               *
* bool existPath(const char*)                                        *
* const char* getTimeStamp(const char*)                              *
* bool imageExists(const char*)                                      *
* template<int B, typename Func> bool exeCommand(const char*, Func)  *
* std::string mkScreenshotStrCmd(const char*, const char*, const char*);
* std::string mkScreenshotStrCmd(const char*, const char*, bool manual);
* bool takeScreenshot(const char*, const char*, const char*);        *
* bool takeScreenshot(const char*, const char*, bool);               *
* bool wxTakeScreenshot(const int, const char*, const char*, const char*);
* bool wxTakeScreenshot(const int, const char*, const char*, bool manual);
* bool isRGB(const char*);                                           *
* bool isHex(const char*);                                           *
* template<typename T> struct ToString                               *
* template<typename T, typename... Args> std::string ToString2(T , Args...)
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef UTILITIES_H
#define UTILITIES_H

#include "debug_utils.h"

#include <cstring>
#include <string>
#include <functional>
#include <memory>

#define SEPARATOR "#+|+#"

//====================================================================

bool cstrCompare(const char* str1, const char* str2);

int findStr(const char* needle, const char* stack, int offset=0);

//====================================================================

bool windowExists(const char* windowName);

//====================================================================

struct WindowRect
{
	WindowRect(int x, int y, int w, int h)
	:m_x(x), m_y(y), m_w(w), m_h(h)
	{}

	// if point (x, y) is absolute
	bool contains(int x, int y)
	{
		if(x>m_x && x<m_x+m_w){
			return (y>m_y && y<m_y+m_h);
		}
		return false;
	}

	// if the point (x, y) is relative to the WindowRect
	bool inside(int x, int y)
	{
		if(x<m_w){
			return (y<m_h);
		}
		return false;
	}

	const int m_x;
	const int m_y;
	const int m_w;
	const int m_h;
};

WindowRect getWindowRect(const char* windowName, bool visible);

std::string getWindowROI(const char* windowName);


//====================================================================

std::pair<int, int> getWindowCoord(const char* windowName, int x, int y, const char* position="Absolute");

//====================================================================

inline std::pair<int, int> getWindowCoord(const char* windowName, const char* position="Absolute")
{
	return getWindowCoord(windowName, 0, 0, position);
}

//====================================================================

std::string resourcePath(const char* fileName);

//====================================================================

std::string getFilePath(const char* file="");

//====================================================================

std::string getImgPath(const char* file="");

//====================================================================

inline std::string getImgPath(const std::string& file)
{
	return getImgPath(file.c_str());
}

//====================================================================

bool existPath(const char* fileName);

//====================================================================

const char* getTimeStamp(const char* format="%Y-%m-%dT%H:%M:%SZ");

bool imageExists(const char* imageName);

inline bool imageExists(const std::string& imageName)
{
	return imageExists(imageName.c_str());
}

bool removeImage(const char* imageName);

inline bool removeImage(const std::string& imageName)
{
	return removeImage(imageName.c_str());
}

//====================================================================

template<int B, typename Func>
bool exeCommand(const char* cmd, Func cbk)
{
	std::string str(" ", B);
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
	if (!pipe) {
		return false;
	}

	char* buff=str.data();
	std::memset(buff, 0, B);
	while(fgets(buff, B-1, pipe.get()) != nullptr) {
		
		size_t pos=str.find_first_of('\n');
		if(pos!=std::string::npos){
			buff[pos]=0;
		}
		cbk(str);
		std::memset(buff, 0, B);
	}
	return true;
}

//====================================================================
/*
//https://en.cppreference.com/w/cpp/io/c/fprintf
template<typename... Args>
std string formattedString(const char* format, Args... args);
int snprintf( char* buffer, std::size_t buf_size, const char* format, ... );
// */

std::string mkScreenshotStrCmd(const char* windowName, const char* outputImage, const char* roiStr);

std::string mkScreenshotStrCmd(const char* windowName, const char* outputImage, bool manual=false);

inline std::string mkScreenshotStrCmd(const std::string& windowName, const std::string& outputImage, const std::string& roiStr)
{
	return mkScreenshotStrCmd(windowName.c_str(), outputImage.c_str(), roiStr.c_str());
}

inline std::string mkScreenshotStrCmd(const std::string& windowName, const std::string& outputImage, bool manual=false)
{
	return mkScreenshotStrCmd(windowName.c_str(), outputImage.c_str(), manual);
}

//====================================================================

bool takeScreenshot(const char* windowName, const char* outputImage, const char* roiStr);

bool takeScreenshot(const char* windowName, const char* outputImage, bool manual=false);

inline bool takeScreenshot(const std::string& windowName, const std::string& outputImage, const std::string& roiStr)
{
	return takeScreenshot(windowName.c_str(), outputImage.c_str(), roiStr.c_str());
}

inline bool takeScreenshot(const std::string& windowName, const std::string& outputImage, bool manual=false)
{
	return takeScreenshot(windowName.c_str(), outputImage.c_str(), manual);
}

//====================================================================

bool wxTakeScreenshot(const int ms, const char* windowName, const char* outputImage, const char* roiStr);

bool wxTakeScreenshot(const int ms, const char* windowName, const char* outputImage, bool manual=false);

inline bool wxTakeScreenshot(const int ms, const std::string& windowName, const std::string& outputImage, const std::string& roiStr)
{
	return wxTakeScreenshot(ms, windowName.c_str(), outputImage.c_str(), roiStr.c_str());
}

inline bool wxTakeScreenshot(const int ms, const std::string& windowName, const std::string& outputImage, bool manual=false)
{
	return wxTakeScreenshot(ms, windowName.c_str(), outputImage.c_str(), manual);
}

//====================================================================

bool isRGB(const char* str);

bool isHex(const char* str);

//====================================================================

typedef std::function<std::string()> StrCmd;

template<typename T>
struct ToString
{
	static std::string toStr(T t)
	{
		try {
			return std::to_string(t);
		}
		catch (...) {
			return std::string("!!");
		}
	}
};

template<>
struct ToString<size_t>
{
	static std::string toStr(size_t t)
	{
		return std::to_string(t);
	}
};

template<>
struct ToString<float>
{
	static std::string toStr(float t)
	{
		return std::to_string(t);
	}
};

template<>
struct ToString<int>
{
	static std::string toStr(int t)
	{
		return std::to_string(t);
	}
};

template<>
struct ToString<std::string>
{
	static std::string toStr(const std::string& str)
	{
		return str;
	}
};

template<>
struct ToString<const char*>
{
	static std::string toStr(const char* cstr)
	{
		return cstr;
	}
};

template<>
struct ToString<bool>
{
	static std::string toStr(bool a)
	{
		if(a){
			return std::string("true");
		}
		return std::string("false");
	}
};

template<typename T>
std::string ToString2(T t)
{
	std::string str;
	str.append(ToString<T>::toStr(t));
	return str;
}

template<typename S, typename T>
std::string ToString2(S s, T t)
{
	std::string str;
	str.append(ToString<S>::toStr(s));
	str.append(SEPARATOR);
	str.append(ToString<T>::toStr(t));
	return str;
}

template<typename T, typename... Args>
std::string ToString2(T t, Args... args)
{
	std::string str;
	str.append(ToString<T>::toStr(t));
	str.append(SEPARATOR);
	str.append(ToString2(args...));
	return str;
}

//====================================================================

#endif
