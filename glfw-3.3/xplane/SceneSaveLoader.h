#pragma once
#include <iomanip>

#include "bmpWriter.h"
#include "scene.h"
#include "Object.h"
#include "objectBuilder.h"
#include "lightBuilder.h"
class SceneSaveLoader final
{
public:
	static void loadScene(Scene& scene, std::string path)
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
				auto obj = new Object();			
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
			json jsonObject;
			saveObjectToJson(object, jsonObject);

			j["objects"].push_back(jsonObject);
		}
		for (auto object : scene.lights)
		{
			json jsonLight;
			savePointLightToJson(object, jsonLight);

			j["lights"].push_back(jsonLight);
		}
		for (auto object : scene.cameras)
		{
			json jsonCamera;
			saveCameraToJson(object, jsonCamera);
			j["cameras"].push_back(jsonCamera);
		}
		std::ofstream s(scene.sceneName);
		s << std::setw(4)<< j<<std::endl;
	}
protected:

	static void saveObjectToJson(std::vector<Object*>::value_type object, nlohmann::json jsonObject)
	{
		jsonObject["name"] = object->name;
		auto mat = object->getComponent<Material>();
		jsonObject["color"] = { mat->diffuseColor.r, mat->diffuseColor.g, mat->diffuseColor.b };
		jsonObject["roughness"] = mat->roughness;
		jsonObject["transparency"] = mat->transparency;
		jsonObject["ior"] = mat->ior;
		auto trasform = object->getComponent<Transform>();
		jsonObject["position"] = { trasform->position.x, trasform->position.y, trasform->position.z };
	}

	static void savePointLightToJson(std::vector<Object*>::value_type object, nlohmann::json jsonLight)
	{
		auto light = object->getComponent<PointLight>();
		jsonLight["color"] = { light->color.r, light->color.g, light->color.b };
		jsonLight["intensity"] = light->intensity;
		auto trasform = object->getComponent<Transform>();
		jsonLight["position"] = { trasform->position.x, trasform->position.y, trasform->position.z };
	}

	static void saveCameraToJson(std::vector<Object*>::value_type object, nlohmann::json jObj)
	{
		auto trasform = object->getComponent<Transform>();
		jObj["position"] = { trasform->position.x, trasform->position.y, trasform->position.z };
		jObj["fov"] = object->getComponent<Camera>()->Zoom;
	}
};


