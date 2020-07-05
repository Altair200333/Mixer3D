#pragma once
#include "camera.h"
#include "objectBuilder.h"
#include "scene.h"

class CameraBuilder final:public ObjectBuilder
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
	value["fov"] = object->getComponent<Camera>()->zoom;
	auto front = object->getComponent<Camera>()->Front;
	value["front"] = {front.x, front.y, front.z};
	auto right = object->getComponent<Camera>()->Right;
	value["right"] = { right.x, right.y, right.z };
	auto up = object->getComponent<Camera>()->Up;
	value["up"] = { up.x, up.y, up.z };
	value["yaw"] = object->getComponent<Camera>()->yaw;
	value["pitch"] = object->getComponent<Camera>()->pitch;
}

inline Object* CameraBuilder::fromJson(nlohmann::json& value, Scene& scene)
{
	auto obj = new Object();
	glm::vec3 position = getVec3Or(value, "position", { 0,0,-1 });
	obj->addComponent(new Transform(obj, position))->addComponent(new Camera(obj, static_cast<float>(scene.window->width) / scene.window->height,
		static_cast<float>(value.at("fov"))));
	auto camera = obj->getComponent<Camera>();
	camera->Front = getVec3Or(value, "front", camera->Front);
	camera->Right = getVec3Or(value, "right", camera->Right);
	camera->Up = getVec3Or(value, "up", camera->Up);
	camera->yaw = getFloatOr(value, "yaw", camera->yaw);
	camera->pitch = getFloatOr(value, "pitch", camera->pitch);
	return obj;
}