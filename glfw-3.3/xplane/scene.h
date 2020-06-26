#pragma once
//#include <glad/glad.h>

#include <nlohmann/json.hpp>

#include "camera.h"
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

	void AddObject(Object* object)
	{
		objects.push_back(object);
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