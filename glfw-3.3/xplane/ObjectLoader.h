#pragma once
#include "Object.h"
#include "window.h"
#include <string>

class ObjectLoader
{
public:
	static Object* loadObject(std::string path, Window* window);
};
