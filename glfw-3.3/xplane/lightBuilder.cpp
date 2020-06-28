#include "lightBuilder.h"

LightBuilder& LightBuilder::addLight(glm::vec3 color, float intensity)
{
	obj->addComponent(new PointLight(obj, color, intensity));
	
	return *this;
}

LightBuilder& LightBuilder::addStandartSphere(glm::vec3 color, Window* window)
{
	addMesh("icosphere.stl").addRenderer(window).addMaterial(color, 1, 1).addTransform();
	
	return *this;
}

Object* LightBuilder::fromJson(nlohmann::json& value, Window* window)
{
	glm::vec3 color = getVec3Or(value, "color", { 1,1,1 });
	glm::vec3 position = getVec3Or(value, "position", { 0,0,0 });

	Object* obj = ObjectBuilder().addMesh("icosphere.stl").addRenderer(window).addMaterial(color, 1, 1);
	obj->addComponent(new Transform(obj, position))->addComponent(new PointLight(obj, color, value.at("intensity")));

	return obj;
}

void LightBuilder::toJson(Object* object, nlohmann::json& jsonLight)
{
	auto light = object->getComponent<PointLight>();
	jsonLight["color"] = { light->color.r, light->color.g, light->color.b };
	jsonLight["intensity"] = light->intensity;
	auto trasform = object->getComponent<Transform>();
	jsonLight["position"] = { trasform->position.x, trasform->position.y, trasform->position.z };
}
