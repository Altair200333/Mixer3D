#pragma once
#include <glm/glm/vec3.hpp>
#include <nlohmann/json.hpp>



#include "itemBuilder.h"
#include "LightSource.h"
#include "Object.h"
#include "objectBuilder.h"
#include "transform.h"
#include "window.h"

class LightBuilder final: public ObjectBuilder
{
public:
	LightBuilder():ObjectBuilder(){}

	LightBuilder& addLight(glm::vec3 color, float intensity);
	
	LightBuilder& addStandartSphere(glm::vec3 color, Window* window);
	
	static Object* fromJson(nlohmann::json& value, Window* window);
	static void toJson(Object* obj, nlohmann::json& value);
};
