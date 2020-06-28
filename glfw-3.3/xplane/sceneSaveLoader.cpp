#include "cameraBuilder.h"
#include "SceneSaveLoader.h"

void SceneSaveLoader::loadScene(Scene& scene, std::string path)
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

void SceneSaveLoader::fromJson(Scene& scene, nlohmann::json& j)
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
			scene.addObject(ObjectBuilder().fromJson(value, scene.window));

	if (j.find("lights") != j.end())
		for (auto& [key, value] : j.at("lights").items())
		{
			scene.lights.push_back(LightBuilder().fromJson(value, scene.window));
		}
	if (j.find("cameras") != j.end())
		for (auto& [key, value] : j.at("cameras").items())
		{
			scene.cameras.push_back(CameraBuilder().fromJson(value, scene));
		}
}

void SceneSaveLoader::saveScene(Scene& scene)
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
	s << std::setw(4) << j << std::endl;
}

void SceneSaveLoader::saveObjectToJson(Object* object, nlohmann::json& jsonObject)
{
	ObjectBuilder::toJson(object, jsonObject);
}

void SceneSaveLoader::savePointLightToJson(Object* object, nlohmann::json& jsonLight)
{
	LightBuilder::toJson(object, jsonLight);
}

void SceneSaveLoader::saveCameraToJson(Object* object, nlohmann::json& jObj)
{
	CameraBuilder().toJson(object, jObj);
}