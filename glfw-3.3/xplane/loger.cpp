#include "Loger.h"


std::vector<std::string> Logger::logs;

void Logger::log(std::string message)
{
	if (logs.size() > LoggerCapacity)
	{
		logs.erase(logs.begin());
	}
	logs.push_back(message);
}
std::string Logger::getReport()
{
	std::string output;
	for (const auto& str : logs)
	{
		output += str + "\n";
	}
	return output;
}