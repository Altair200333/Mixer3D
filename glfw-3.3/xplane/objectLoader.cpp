#include "ObjectLoader.h"
#include "fileManager.h"
#include "Loger.h"
#include "objectBuilder.h"

Object* ObjectLoader::loadObject(std::string path, Window* window)
{
	if (path.empty())
	{
		Logger::log("No file selected");
		return nullptr;
	}
	else if (path.find(".stl") != path.npos && FileManager::fileExists(path))
	{
		Object* obj = ObjectBuilder().addMesh(path).addRenderer(window).addMaterial().addTransform();
		Logger::log("Object created");
		return obj;
	}
	Logger::log("Failed to load object");
	return nullptr;
}