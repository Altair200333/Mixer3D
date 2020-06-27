#pragma once
#include <string>
#include <vector>

#define LoggerCapacity 20
class Logger final
{
	static std::vector<std::string> logs;
public:
	Logger() = delete;
	
	static void log(std::string message)
	{
		if (logs.size() > LoggerCapacity)
		{
			logs.erase(logs.begin());
		}
		logs.push_back(message);
	}
	static std::string getReport()
	{
		std::string output;
		for (const auto& str : logs)
		{
			output += str + "\n";
		}
		return output;
	}
};
