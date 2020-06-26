#pragma once
//#include <glad/glad.h>

#include <nlohmann/json.hpp>

#include "camera.h"
#include "fileManager.h"
#include "Loger.h"
#include "Object.h"
#include "window.h"
//Scene class is more of a container that has information about objects, materials, lighting, cameras etc
class Scene
{
public:
	int maxBounces = 2;
	std::vector<Object*> objects;
	std::vector<Object*> lights;
	std::vector<Object*> cameras;
	std::string sceneName = "Untitled.mxr";
	
	std::string envPath;
	Bitmap* environment = nullptr;
	Window* window;
	Scene(Window* _window):window(_window)
	{
	}

	void loadEnvironment(std::string path)
	{
		BMPWriter bmpw;
		if (FileManager::fileExists(path) && path.find(".jpg") != path.npos)
		{
			if (environment != nullptr)
				environment->clear();
			environment = bmpw.loadJPG(path);
			envPath = path;
			Logger::log("Environment texture loaded");
		}
		else
		{
			Logger::log("Failed to load image");
		}
	}
	void AddObject(Object* object)
	{
		if (object != nullptr)
			objects.push_back(object);
		else
			Logger::log("Failed to insert null object");
	}
	void deleteObject(Object* obj)
	{
		auto it = std::find(objects.begin(), objects.end(), obj);
		delete obj;
		objects.erase(it);
	}
	Camera* getActiveCamera()
	{
		return cameras[0]->getComponent<Camera>();
	}
	virtual ~Scene() = default;
	void clear()
	{
		if(environment!=nullptr)
			environment->clear();
		for (auto obj : objects)
			delete obj;
		objects.clear();
		for (auto obj : lights)
			delete obj;
		lights.clear();
		for (auto cam : cameras)
			delete cam;
		cameras.clear();
	}
	
};