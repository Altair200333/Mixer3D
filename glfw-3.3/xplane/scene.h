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

	Scene(int w, int h):width(w), height(h)
	{
		auto camera = new Object();
		camera->addComponent(new Transform(camera, glm::vec3(0.0f, 0.0f, 3.0f)))->addComponent(new Camera( static_cast<float>(w) / h,camera));

		cameras.push_back(camera);
		auto light = new Object();
		light->addComponent(new Transform(light, { 13,13,13 }))->addComponent(new PointLight(light, { 1,1,1 }, 1));
		lights.push_back(light);
	}

	void AddObject(Object* object)
	{
		objects.push_back(object);
	}

	Camera* getActiveCamera()
	{
		return cameras[0]->getComponent<Camera>();
	}
};

