#pragma once
#include <string>
#include <vector>

#define LoggerCapacity 20
class Logger final
{
	static std::vector<std::string> logs;
public:
	Logger() = delete;
	
	static void log(std::string message);
	static std::string getReport();
};
