#pragma once
//#include <glad/glad.h>

#include <nlohmann/json.hpp>

#include "camera.h"
#include "fileManager.h"
#include "Loger.h"
#include "Object.h"
#include "window.h"
#include "bmpWriter.h"
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

	void loadEnvironment(std::string path);
	void AddObject(Object* object);
	void deleteObject(Object* obj);
	Camera* getActiveCamera();
	virtual ~Scene() = default;
	void clear();
	
};