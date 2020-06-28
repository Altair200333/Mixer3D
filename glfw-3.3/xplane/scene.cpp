#include "scene.h"

namespace 
{
	bool addObjectToVector(Object* object, std::vector<Object*>& list)
	{
		if(object == nullptr)
		{
			return false;
		}
		list.push_back(object);
		return true;
	}
	void deleteObjectFromContainer(Object* object, std::vector<Object*>& list)
	{
		const auto it = std::find(list.begin(), list.end(), object);
		delete object;
		list.erase(it);
	}
}
void Scene::loadEnvironment(std::string path)
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
void Scene::addObject(Object* object)
{
	if(!addObjectToVector(object, objects))
		Logger::log("Failed to insert null object");
}
void Scene::deleteObject(Object* obj)
{
	deleteObjectFromContainer(obj, objects);
}
void Scene::deleteLight(Object* obj)
{
	deleteObjectFromContainer(obj, lights);
}
void Scene::deleteCamera(Object* obj)
{
	deleteObjectFromContainer(obj, cameras);
}
void Scene::addLight(Object* object)
{
	if (!addObjectToVector(object, lights))
		Logger::log("Failed to insert null light");
}
void Scene::addCamera(Object* object)
{
	if (!addObjectToVector(object, cameras))
		Logger::log("Failed to insert null light");
}
void Scene::setActiveCamera(Object* object)
{
	if (object == nullptr)
		return;
	std::iter_swap(cameras.begin(), std::find(cameras.begin(), cameras.end(), object));
}
Camera* Scene::getActiveCamera()
{
	return !cameras.empty() ? cameras[0]->getComponent<Camera>() : nullptr;
}

Scene::~Scene()
{
	clear();
}

void Scene::clear()
{
	if (environment != nullptr)
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
