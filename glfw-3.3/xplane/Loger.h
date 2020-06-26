#pragma once
#include <list>
#include <string>

class Logger final
{
public:
	static std::vector<std::string> logs;

	static void log(std::string message)
	{
		if(logs.size()>10)
		{
			logs.erase(logs.begin());
		}
		logs.push_back(message);
	}
	static std::string getReport()
	{
		std::string output;
		for(auto str: logs)
		{
			output += str + "\n";
		}
		return output;
	}
};
std::vector<std::string> Logger::logs;