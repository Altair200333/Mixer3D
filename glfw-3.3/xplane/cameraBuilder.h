#pragma once
#include "camera.h"
#include "objectBuilder.h"
#include "scene.h"

class CameraBuilder:public ObjectBuilder
{
public:
	CameraBuilder() :ObjectBuilder() {}

	CameraBuilder& addCamera(float ar, float fov)
	{
		obj->addComponent(new Camera(obj, ar, fov));
		return *this;
	}
	
	static void toJson(Object* obj, nlohmann::json& value);
	Object* fromJson(nlohmann::json& value, Scene& scene);
};

inline void CameraBuilder::toJson(Object* object, nlohmann::json& value)
{
	auto transform = object->getComponent<Transform>();
	value["position"] = { transform->position.x, transform->position.y, transform->position.z };
	value["fov"] = object->getComponent<Camera>()->Zoom;
}

inline Object* CameraBuilder::fromJson(nlohmann::json& value, Scene& scene)
{
	auto obj = new Object();
	glm::vec3 position = getVec3Or(value, "position", { 0,0,-1 });
	obj->addComponent(new Transform(obj, position))->addComponent(new Camera(obj, static_cast<float>(scene.window->width) / scene.window->height,
		static_cast<float>(value.at("fov"))));
	return obj;
}