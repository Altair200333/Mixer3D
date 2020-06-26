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
		objects.clear();
		lights.clear();
		cameras.clear();
	}
	void loadSceneFromJson(std::string path)
	{
		clear();
		sceneName = path;
		using json = nlohmann::json;

		std::ifstream i(path);
		json j;
		i >> j;

		BMPWriter bmpw;
		if (j.find("maxBounces") != j.end())
			maxBounces = j.at("maxBounces");
		if (j.find("env") != j.end())
		{
			environment = bmpw.loadJPG(j.at("env"));
			envPath = j.at("env");
		}
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
				auto obj = new Object("Lamp");
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
	void saveScene()
	{
		using json = nlohmann::json;
		json j;
		j["maxBounces"] = maxBounces;
		if (environment != nullptr)
			j["env"] = envPath;

		for(auto object: objects)
		{
			json jObj;
			jObj["name"] = object->name;
			auto mat = object->getComponent<Material>();
			jObj["color"] = {mat->diffuseColor.r, mat->diffuseColor.g, mat->diffuseColor.b};
			jObj["roughness"] = mat->roughness;
			auto trasform = object->getComponent<Transform>();
			jObj["position"] = { trasform->position.x, trasform->position.y, trasform->position.z };

			j["objects"].push_back(jObj);
		}
		for (auto object : lights)
		{
			json jObj;
			auto light = object->getComponent<PointLight>();
			jObj["color"] = { light->color.r, light->color.g, light->color.b };
			jObj["intensity"] = light->intensity;
			auto trasform = object->getComponent<Transform>();
			jObj["position"] = { trasform->position.x, trasform->position.y, trasform->position.z };

			j["lights"].push_back(jObj);
		}
		for (auto object : cameras)
		{
			json jObj;
			auto trasform = object->getComponent<Transform>();
			jObj["position"] = { trasform->position.x, trasform->position.y, trasform->position.z };
			jObj["fov"] = object->getComponent<Camera>()->Zoom;
			j["cameras"].push_back(jObj);
		}
		std::ofstream s(sceneName);
		s << j;
	}
};

