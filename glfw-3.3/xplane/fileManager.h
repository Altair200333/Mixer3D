#pragma once

#include <string>
#include <filesystem>

class FileManager
{
public:
	static bool fileExists(const std::string& name);
	static std::string getPathDialog();
};
