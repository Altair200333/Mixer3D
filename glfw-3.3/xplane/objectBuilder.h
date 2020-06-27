#pragma once
//#include <glm/glm/glm.hpp>


#include <glm/glm/vec3.hpp>


#include "itemBuilder.h"
#include "nlohmann/json.hpp"
#include "Object.h"

class Window;

class ObjectBuilder: public ItemBuilder
{
protected:
	Object* obj;
public:
	ObjectBuilder() { obj = new Object(); }
	operator Object* () const
	{
		return obj;
	}
	ObjectBuilder& addMesh(std::string name);
	ObjectBuilder& addRenderer(Window* window);
	ObjectBuilder& addMaterial();
	ObjectBuilder& addMaterial(glm::vec3 color, float _rough, float _transparency);
	ObjectBuilder& addMaterial(glm::vec3 color, float _rough, float _transparency, float _ior);
	ObjectBuilder& addTransform();
	ObjectBuilder& addTransform(glm::vec3 position);
	
	static Object* fromJson(nlohmann::json& value, Window* window);
	static void toJson(Object* obj, nlohmann::json& value);

};
