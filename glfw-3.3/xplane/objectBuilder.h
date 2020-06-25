#pragma once
#include <glm/glm/vec3.hpp>


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
};
