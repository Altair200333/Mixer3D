#pragma once
#include <glm/glm/vec3.hpp>
#include <nlohmann/json.hpp>


#include "LightSource.h"
#include "Object.h"
#include "objectBuilder.h"
#include "transform.h"
#include "window.h"

class LightBuilder final
{
	Object* obj;
public:
	LightBuilder() { obj = new Object(); }

	LightBuilder& addTransform()
	{
		addTransform({ 0,0,0 });
		return *this;
	}
	LightBuilder& addTransform(glm::vec3 position)
	{
		obj->addComponent<Transform>(new Transform(obj, position));

		return *this;
	}
	operator Object* () const
	{
		return obj;
	}
	static Object* fromJson(nlohmann::json& value, Window* window)
	{
		glm::vec3 color = { value.at("color")[0],value.at("color")[1] ,value.at("color")[2] };
		glm::vec3 position = { value.at("position")[0],value.at("position")[1] ,value.at("position")[2] };

		Object* obj = ObjectBuilder().addMesh("icosphere.stl").addRenderer(window).addMaterial(color, 1, 0);
		obj->addComponent(new Transform(obj, position))->addComponent(new PointLight(obj, color, value.at("intensity")));

		return obj;
	}
};
