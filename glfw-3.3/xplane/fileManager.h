#pragma once

#include <string>
#include <filesystem>

namespace FileManager
{
	bool fileExists(const std::string& name);
	std::string getPathDialog();
};
