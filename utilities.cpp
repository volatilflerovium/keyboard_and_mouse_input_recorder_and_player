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
#include "utilities.h"
#include "cstr_split.h"

#include <wx/string.h>
#include <wx/display.h>
#include <wx/utils.h> 
#include <wx/dc.h>

#include <ctime>
#include <iterator>
#include <locale>
#include <filesystem>

wxColour s_colour(*wxBLUE);

wxBrush s_brush(s_colour, wxBRUSHSTYLE_CROSSDIAG_HATCH);

//====================================================================

bool cstrCompare(const char* str1, const char* str2)
{
	if(nullptr!=str1 && nullptr!=str2){
		const size_t strLength=std::strlen(str1);
		if(strLength==std::strlen(str2)){
			return std::memcmp(str1, str2, strLength)==0;
		}
	}
	return false;
}

int findStr(const char* needle, const char* stack, int offset)
{
	size_t needleLength=std::strlen(needle);
	const int maxPosition=std::strlen(stack)-needleLength;
	if(maxPosition<0){
		return -1;
	}
	int pos=offset;
	while(pos<=maxPosition){
		if(std::memcmp(needle, stack+pos, needleLength)==0){
			return pos;
		}
		pos++;
	}
	return -1;

}

//====================================================================

std::string resourcePath(const char* fileName)
{
	#ifdef DEBUG
	std::string filePath=DEBUG_DIR; // define in CMakeLists
	#else
	std::string filePath=getenv("APPDIR");
	filePath.append("/usr/share");
	#endif
	
	filePath.append("/utility_files/");
	filePath.append(fileName);
	return filePath;
}

//====================================================================

std::string getFilePath(const char* file)
{
	#ifndef DEBUG
	const char* hidDir="/.wxHID";
	std::string dirPath=getenv("HOME");
	#else
	const char* hidDir="/wxHID";
	std::string dirPath=DEBUG_DIR; // define in CMakeLists
	dirPath.append("/debug_dir");
	#endif

	dirPath.append(hidDir);
	
	if(std::strlen(file)>0){
		dirPath.append("/");
		dirPath.append(file);
	}

	return dirPath;	
}

//====================================================================

std::string getImgPath(const char* file)
{
	std::string dirPath=getFilePath();	
	if(std::strlen(file)>0){
		dirPath.append("/img/");
		dirPath.append(file);
	}
	else{
		dirPath.append("/img");
	}
	
	return dirPath;	
}

//====================================================================

bool existPath(const char* fileName)
{
	std::string dirPath=getFilePath(fileName);
	std::error_code ec;
	std::filesystem::path applicationDirPath(dirPath.c_str());
	return std::filesystem::exists(applicationDirPath, ec);
}

//====================================================================

const char* getTimeStamp(const char* format)
{
	static char timeString[32];
	std::time_t time = std::time({});
	std::memset(timeString, 0, 32);
	std::strftime(timeString, 31, format, std::gmtime(&time));
   return timeString;
}

//====================================================================

bool imageExists(const char* imageName)
{
	if(std::strlen(imageName)>0){
		std::error_code ec;
		std::filesystem::path imgPath{getImgPath(imageName)};
		if(std::filesystem::exists(imgPath, ec)){
			return true;
		}
	}
	return false;
}

//====================================================================

bool removeImage(const char* imageName)
{
	if(std::strlen(imageName)>0){
		std::error_code ec;
		std::filesystem::path imgPath{getImgPath(imageName)};
		if(std::filesystem::exists(imgPath, ec)){
			std::filesystem::remove(imgPath);
			return true;
		}
	}
	return false;
}

//====================================================================

bool windowExists(const char* windowName)
{
	if(cstrCompare(windowName, "root")){
		return true;
	}
	auto command=wxString::Format("xprop -name \"%s\" 1>/dev/null 2>&1; echo $?", windowName);
	bool windowExists=true;
	exeCommand<64>(command.mb_str(), [&windowExists](const std::string& str){
		windowExists=(std::atoi(str.c_str())==0);
	});

	return windowExists;
}

//====================================================================

WindowRect getWindowRect(const char* windowName, bool visible)
{
	bool wExists=windowExists(windowName);

	if(wExists){
		const char* data[6]={
			"Absolute upper-left X:",
			"Absolute upper-left Y:", 
			"Relative upper-left X:", 
			"Relative upper-left Y:", 
			"Width:", 
			"Height:"
		};

		auto command=wxString::Format("xwininfo -name \"%s\" 2>&1", windowName);

		int geometry[6];
		int i=0;
		exeCommand<64>(command.mb_str(), [data, &geometry, &i, &wExists](const std::string& str){
			if(i<6){
				try{
					size_t r=str.find(data[i]);
					if(r!=std::string::npos){
						size_t j=str.find(':', r);
						geometry[i++]=std::atoi(str.c_str()+j+1);
					}
				}
				catch(...){
					i=6;
					wExists=false;
				}
			}
		});
		if(wExists){
			
			int x=geometry[0]-geometry[2];
			int y=geometry[1]-geometry[3];
			int w=geometry[4]+2*geometry[2];
			int h=geometry[5]+geometry[3];

			if(visible){
				wxRect rt=wxDisplay().GetGeometry();
				if(w>rt.GetWidth()-x){
					w=rt.GetWidth()-x;
				}
				if(h>rt.GetHeight()-y){
					h=rt.GetHeight()-y;
				}
			}
			return WindowRect(x, y, w, h);
		}
	}
	return WindowRect(0, 0, 0, 0);
}


std::string getWindowROI(const char* windowName)
{
	WindowRect rect=getWindowRect(windowName, true);

	return std::string(wxString::Format("%ix%i+%i+%i", rect.m_w, rect.m_h, rect.m_x, rect.m_y));
}


std::pair<int, int> getWindowCoord(const char* windowName, int x, int y, const char* position)
{
	bool wExists=windowExists(windowName);

	if(wExists){
		auto command=wxString::Format("xwininfo -name \"%s\" | grep \"upper-left\" | cut -d\":\" -f2 2>&1", windowName, position);
		//dbg(command);
		int coordinates[2]={x, y};
		int i=0;
		
		exeCommand<16>(command.mb_str(), [&coordinates, &i, &wExists](const std::string& str){
			try{
				if(wExists){
					if(i<2){
						coordinates[i++]+=std::atoi(str.c_str());
					}
					else{
						coordinates[(i++)-2]-=std::atoi(str.c_str());
					}
				}
			}
			catch(...){
				wExists=false;
			}
		});
		if(wExists){
			return {coordinates[0], coordinates[1]};
		}
	}
	return {-1, -1};
}

//====================================================================

std::string mkScreenshotStrCmd(const char* windowName, const char* outputImage, const char* roiStr)
{
	wxString cropStr;
	if(std::strlen(roiStr)>0){
		cropStr=wxString::Format(wxT("-crop %s "), roiStr);
	}
	else{
		cropStr="";
	}
	
	wxString screenshotCmd;
	if(cstrCompare(windowName, "root")){
		screenshotCmd=wxString::Format(wxT("import -window \"%s\" %s +repage %s -quiet"), windowName, cropStr, getImgPath(outputImage));
	}
	else{
		screenshotCmd=wxString::Format(wxT("import -window \"%s\" -frame %s +repage %s -quiet"), windowName, cropStr, getImgPath(outputImage));
	}

	return std::string(screenshotCmd.mb_str());
}

std::string mkScreenshotStrCmd(const char* windowName, const char* outputImage, bool manual)
{
	if(!manual){
		return mkScreenshotStrCmd(windowName, outputImage, "");
	}
	std::string roiStr=getWindowROI(windowName);
	if(roiStr!="0x0+0+0"){
		return mkScreenshotStrCmd("root", outputImage, roiStr.c_str());
	}
	return "";
}


//====================================================================

bool takeScreenshot(const char* windowName, const char* outputImage, const char* roiStr)
{
	if(windowExists(windowName)){
		return 0==system(mkScreenshotStrCmd(windowName, outputImage, roiStr).c_str());
	}
	return false;
}

bool takeScreenshot(const char* windowName, const char* outputImage, bool manual)
{
	if(windowExists(windowName)){
		return 0==system(mkScreenshotStrCmd(windowName, outputImage, manual).c_str());
	}
	return false;
}

bool wxTakeScreenshot(const int ms, const char* windowName, const char* outputImage, const char* roiStr)
{
	if(windowExists(windowName)){
		int i=0;
		while(i<ms){
			wxMilliSleep(50);
			i+=50;
		}
		return 0==system(mkScreenshotStrCmd(windowName, outputImage, roiStr).c_str());
	}
	return false;
}

bool wxTakeScreenshot(const int ms, const char* windowName, const char* outputImage, bool manual)
{
	if(windowExists(windowName)){
		int i=0;
		while(i<ms){
			wxMilliSleep(50);
			i+=50;
		}
		return 0==system(mkScreenshotStrCmd(windowName, outputImage, manual).c_str());
	}
	return false;
}


//====================================================================


bool isRGB(const char* str)
{
	CstrSplit<3> parts(str, ",");
	if(parts.dataSize()!=3){
		return false;
	}
	try
	{
		for(int i=0; i<3; i++){
			int r=std::atoi(parts[i]);
			if(r<0 || r>255){
				return false;
			}
		}
	}
	catch(...){
		return false;
	}

	return true;
}


bool isHex(const char* str)
{
	size_t l=std::strlen(str);
	if(l!=7){
		return false;
	}
	if(str[0]!='#'){
		return false;
	}

	const char* const hex="0123456789ABCDEFabcdef";

	bool a=false;
	for(size_t i=1; i<l; i++){
		a=false;
		for(size_t j=0; j<std::strlen(hex); j++){
			if(str[i]==hex[j]){
				a=true;
				break;
			}
		}
		if(!a){
			return false;
		}
	}

	return true;
}

//====================================================================

