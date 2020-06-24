#pragma once
#include <glad/glad.h>

#include "camera.h"
#include "Object.h"

//Scene class is more of a container that has information about objects, materials, lighting, cameras etc
class Scene
{
public:
	int width, height;
	std::vector<Object*> objects;
	std::vector<LightSource> lights;
	std::vector<Object*> cameras;

	Scene(int w, int h):width(w), height(h)
	{
		auto camera = new Object();
		camera->addComponent<Transform>(new Transform(camera, glm::vec3(0.0f, 0.0f, 3.0f)))->addComponent<Camera>(new Camera( static_cast<float>(w) / h,camera));
		
		cameras.push_back(camera);
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

