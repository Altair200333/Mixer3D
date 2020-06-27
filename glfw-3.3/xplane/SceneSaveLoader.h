#pragma once
#include <iomanip>

#include "bmpWriter.h"
#include "scene.h"
#include "Object.h"
#include "objectBuilder.h"
#include "lightBuilder.h"

class SceneSaveLoader final: public ItemBuilder
{
public:
	static void loadScene(Scene& scene, std::string path);

	static void saveScene(Scene& scene);
protected:
	static void fromJson(Scene& scene, nlohmann::json& j);
	static void saveObjectToJson(Object* object, nlohmann::json& jsonObject);
	static void savePointLightToJson(Object* object, nlohmann::json& jsonLight);
	static void saveCameraToJson(Object* object, nlohmann::json& jObj);
};


