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
class Scene final
{
public:
	int maxBounces = 2;
	int renderEngine = 0;
	
	std::vector<Object*> objects;
	std::vector<Object*> lights;
	std::vector<Object*> cameras;
	std::string sceneName = "Untitled.mxr";
	
	std::string envPath;
	Bitmap* environment = nullptr;
	Window* window;
	Scene(Window* _window):window(_window){}

	void loadEnvironment(std::string path);
	void addObject(Object* object);
	void addLight(Object* object);
	void addCamera(Object* object);
	void setActiveCamera(Object* object);
	void deleteObject(Object* obj);
	void deleteLight(Object* obj);
	void deleteCamera(Object* obj);
	Camera* getActiveCamera();
	~Scene();
	void clear();

	Scene& operator=(Scene& other) = delete;

};