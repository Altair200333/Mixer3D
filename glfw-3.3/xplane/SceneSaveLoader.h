#pragma once
#include <iomanip>

#include "bmpWriter.h"
#include "scene.h"
#include "Object.h"
#include "objectBuilder.h"
#include "lightBuilder.h"
class SceneSaveLoader
{
	
public:
	static void loadScene(Scene& scene, std::string path);

	static void fromJson(Scene& scene, nlohmann::json& j)
	{
		scene.clear();
		using json = nlohmann::json;

		if (j.find("maxBounces") != j.end())
			scene.maxBounces = j.at("maxBounces");
		if (j.find("env") != j.end())
		{
			scene.loadEnvironment(j.at("env"));
		}
		if (j.find("objects") != j.end())
			for (auto& [key, value] : j.at("objects").items())
				scene.AddObject(ObjectBuilder().fromJson(value, scene.window));

		if (j.find("lights") != j.end())
			for (auto& [key, value] : j.at("lights").items())
			{
				scene.lights.push_back(LightBuilder().fromJson(value, scene.window));
			}
		if (j.find("cameras") != j.end())
			for (auto& [key, value] : j.at("cameras").items())
			{
				Object* obj = new Object();
				
				glm::vec3 position = { value.at("position")[0],value.at("position")[1] ,value.at("position")[2] };
				obj->addComponent(new Transform(obj, position))->addComponent(new Camera(obj, static_cast<float>(scene.window->width) / scene.window->height, (float)value.at("fov")));
				scene.cameras.push_back(obj);
			}
	}
	static void saveScene(Scene& scene)
	{
		using json = nlohmann::json;
		json j;
		j["maxBounces"] = scene.maxBounces;
		if (scene.environment != nullptr)
			j["env"] = scene.envPath;

		for (auto object : scene.objects)
		{
			json jObj;
			jObj["name"] = object->name;
			auto mat = object->getComponent<Material>();
			jObj["color"] = { mat->diffuseColor.r, mat->diffuseColor.g, mat->diffuseColor.b };
			jObj["roughness"] = mat->roughness;
			jObj["transparency"] = mat->transparency;
			jObj["ior"] = mat->ior;
			auto trasform = object->getComponent<Transform>();
			jObj["position"] = { trasform->position.x, trasform->position.y, trasform->position.z };

			j["objects"].push_back(jObj);
		}
		for (auto object : scene.lights)
		{
			json jObj;
			auto light = object->getComponent<PointLight>();
			jObj["color"] = { light->color.r, light->color.g, light->color.b };
			jObj["intensity"] = light->intensity;
			auto trasform = object->getComponent<Transform>();
			jObj["position"] = { trasform->position.x, trasform->position.y, trasform->position.z };

			j["lights"].push_back(jObj);
		}
		for (auto object : scene.cameras)
		{
			json jObj;
			auto trasform = object->getComponent<Transform>();
			jObj["position"] = { trasform->position.x, trasform->position.y, trasform->position.z };
			jObj["fov"] = object->getComponent<Camera>()->Zoom;
			j["cameras"].push_back(jObj);
		}
		std::ofstream s(scene.sceneName);
		s << std::setw(4)<< j<<std::endl;
	}
};

inline void SceneSaveLoader::loadScene(Scene& scene, std::string path)
{
	if (!path.empty() && FileManager::fileExists(path) && path.find(".mxr") != path.npos)
	{
		scene.clear();
		scene.sceneName = path;

		std::ifstream i(path);
		using json = nlohmann::json;
		json j;
		i >> j;

		fromJson(scene, j);
		Logger::log("Scene from file");
	}
	else
	{
		Logger::log("Wrong extension of file does not exist");
	}
}
