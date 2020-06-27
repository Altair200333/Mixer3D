#pragma once
#include <list>
#include <string>

#define LoggerCapacity 20
class Logger final
{
public:
	static std::vector<std::string> logs;

	static void log(std::string message)
	{
		if(logs.size()> LoggerCapacity)
		{
			logs.erase(logs.begin());
		}
		logs.push_back(message);
	}
	static std::string getReport()
	{
		std::string output;
		for(const auto& str: logs)
		{
			output += str + "\n";
		}
		return output;
	}
};
std::vector<std::string> Logger::logs;