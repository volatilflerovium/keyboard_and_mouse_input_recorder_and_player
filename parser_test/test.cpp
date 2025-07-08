#include <string>
#include <cstring>
#include <fstream>

#include "nlohmann/json.hpp"
#include "cstr_split.h"
#include "profiler.h"

Profiler<std::chrono::microseconds> profiler;

//====================================================================

void loadDataFileJson(const char* fileName, std::function<void(const char*)> cbk)
{
	std::ifstream commandFiles;
	commandFiles.open(fileName, std::ifstream::in);
	if(commandFiles.is_open()){
		std::string commandLine;
		while(std::getline(commandFiles, commandLine)){
			try{
				if(commandLine.length()==0){
					continue;
				}

				cbk(commandLine.c_str());				
			}
			catch(...){
				break;
			}
		}
		commandFiles.close();
	}
}

//====================================================================

int main(int argc, char* argv[])
{
	if(argc<2){
		std::cout<<"Options: json or csplit\n";
		return 0;
	}

	if(std::memcmp(argv[1], "json", 4)==0){
		std::cout<<"Parsing with json lib\n\n";
		loadDataFileJson(JSON_DATA_FILE, [](const char* line){
			profiler.start();
			nlohmann::json jsonData=nlohmann::json::parse(line);
			profiler.stop();
		});
		return 0;
	}
	
	std::cout<<"Parsing with cstr_split lib\n\n";
	loadDataFileJson(CSPLIT_DATA_FILE, [](const char* line){
		profiler.start();
		SimpleUnserialization<20> cmdData(line, SEPARATOR);
		profiler.stop();
	});

	return 0;
};
