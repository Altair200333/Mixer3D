#pragma once
//#include <glad/glad.h>

#include "camera.h"
#include "LightSource.h"
#include "Object.h"

//Scene class is more of a container that has information about objects, materials, lighting, cameras etc
class Scene
{
public:
	int width, height;
	int maxBounces;
	std::vector<Object*> objects;
	std::vector<Object*> lights;
	std::vector<Object*> cameras;
	Bitmap environment;
	Scene(int w, int h):width(w), height(h)
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
	virtual ~Scene()
	{
		environment.clear();
	}
};

