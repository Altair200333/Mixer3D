#pragma once
//#include <glad/glad.h>

#include <nlohmann/json.hpp>

#include "camera.h"
#include "Object.h"
#include "objectBuilder.h"
#include "window.h"
//Scene class is more of a container that has information about objects, materials, lighting, cameras etc
class Scene
{
public:
	int maxBounces = 2;
	std::vector<Object*> objects;
	std::vector<Object*> lights;
	std::vector<Object*> cameras;
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
		objects.clear();
		lights.clear();
		cameras.clear();
	}
	void loadSceneFromJson(std::string name)
	{
		clear();
		using json = nlohmann::json;

		std::ifstream i(name);
		json j;
		i >> j;

		BMPWriter bmpw;
		if (j.find("maxBounces") != j.end())
			maxBounces = j.at("maxBounces");
		if (j.find("env") != j.end())
			environment = bmpw.loadJPG(j.at("env"));
		if (j.find("objects") != j.end())
			for (auto& [key, value] : j.at("objects").items())
			{
				glm::vec3 color = { value.at("color")[0],value.at("color")[1] ,value.at("color")[2] };
				glm::vec3 position = { value.at("position")[0],value.at("position")[1] ,value.at("position")[2] };
				AddObject(ObjectBuilder().addMesh(value.at("name")).addRenderer(window).addMaterial(color, value.at("roughness")).addTransform(position));
			}
		if (j.find("lights") != j.end())
			for (auto& [key, value] : j.at("lights").items())
			{
				auto obj = new Object();
				glm::vec3 color = { value.at("color")[0],value.at("color")[1] ,value.at("color")[2] };
				glm::vec3 position = { value.at("position")[0],value.at("position")[1] ,value.at("position")[2] };

				obj = ObjectBuilder().addMesh("icosphere.stl").addRenderer(window).addMaterial(color, 1);
				obj->addComponent(new Transform(obj, position))->addComponent(new PointLight(obj, color, value.at("intensity")));
				lights.push_back(obj);
			}
		for (auto& [key, value] : j.at("cameras").items())
		{
			auto obj = new Object();
			glm::vec3 position = { value.at("position")[0],value.at("position")[1] ,value.at("position")[2] };
			obj->addComponent(new Transform(obj, position))->addComponent(new Camera(obj, static_cast<float>(window->width) / window->height, value.at("fov")));
			cameras.push_back(obj);
		}

	}
};

