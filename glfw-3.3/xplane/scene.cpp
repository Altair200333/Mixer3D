#include "scene.h"


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
	if (object != nullptr)
		objects.push_back(object);
	else
		Logger::log("Failed to insert null object");
}
void Scene::deleteObject(Object* obj)
{
	auto it = std::find(objects.begin(), objects.end(), obj);
	delete obj;
	objects.erase(it);
}
void Scene::deleteLight(Object* obj)
{
	auto it = std::find(lights.begin(), lights.end(), obj);
	delete obj;
	lights.erase(it);
}
void Scene::deleteCamera(Object* obj)
{
	auto it = std::find(cameras.begin(), cameras.end(), obj);
	delete obj;
	cameras.erase(it);
}
void Scene::addLight(Object* object)
{
	if (object != nullptr)
		lights.push_back(object);
	else
		Logger::log("Failed to insert null light");
}
void Scene::addCamera(Object* object)
{
	if (object != nullptr)
		cameras.push_back(object);
	else
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