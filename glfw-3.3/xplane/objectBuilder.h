#pragma once
//#include <glm/glm/glm.hpp>


#include "material.h"
#include "MeshRenderer.h"
#include "Object.h"
#include "transform.h"

class ObjectBuilder final
{
	Object* obj;
public:
	ObjectBuilder() { obj = new Object(); }

	ObjectBuilder& addMesh(std::string name)
	{
		obj->name = name;
		obj->addComponent<Mesh>(new Mesh(name, obj));
		return *this;
	}
	ObjectBuilder& addRenderer(Window* window)
	{
		obj->addComponent<MeshRenderer>(new MeshRenderer(window, obj));

		return *this;
	}
	ObjectBuilder& addMaterial()
	{
		obj->addComponent<Material>(new Material(obj));

		return *this;
	}
	ObjectBuilder& addMaterial(glm::vec3 color, float _rough)
	{
		obj->addComponent<Material>(new Material(obj, color, _rough));

		return *this;
	}
	ObjectBuilder& addTransform()
	{
		addTransform({ 0,0,0 });
		return *this;
	}
	ObjectBuilder& addTransform(glm::vec3 position)
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
		Object* obj = ObjectBuilder().addMesh(value.at("name")).addRenderer(window).addMaterial(color, value.at("roughness")).addTransform(position);

		return obj;
	}
};
