#include "objectBuilder.h"
#include "material.h"
#include "MeshLoader.h"
#include "MeshRenderer.h"
#include "transform.h"

ObjectBuilder& ObjectBuilder::addMesh(std::string name)
{
	obj->name = name;
	obj->addComponent<Mesh>(MeshLoader::load(name));
	return *this;
}
ObjectBuilder& ObjectBuilder::addRenderer(Window* window)
{
	obj->addComponent<MeshRenderer>(new MeshRenderer(window, obj));

	return *this;
}
ObjectBuilder& ObjectBuilder::addMaterial()
{
	obj->addComponent<Material>(new Material(obj));

	return *this;
}
ObjectBuilder& ObjectBuilder::addMaterial(glm::vec3 color, float _rough, float _transparency)
{
	obj->addComponent<Material>(new Material(obj, color, _rough, _transparency));

	return *this;
}
ObjectBuilder& ObjectBuilder::addMaterial(glm::vec3 color, float _rough, float _transparency, float _ior)
{
	obj->addComponent<Material>(new Material(obj, color, _rough, _transparency, _ior));

	return *this;
}
ObjectBuilder& ObjectBuilder::addTransform()
{
	addTransform({ 0,0,0 });
	return *this;
}
ObjectBuilder& ObjectBuilder::addTransform(glm::vec3 position)
{
	obj->addComponent<Transform>(new Transform(obj, position));

	return *this;
}

Object* ObjectBuilder::fromJson(nlohmann::json& value, Window* window)
{

	glm::vec3 color = getVec3Or(value, "color", { 0.1,0.7,0.3 });
	glm::vec3 position = getVec3Or(value, "position", {0,0,0});
	Object* obj = ObjectBuilder().addMesh(value.at("name")).addRenderer(window).
		addMaterial(color, getFloatOr(value, "roughness", 0.9), getFloatOr(value, "transparency", 0), getFloatOr(value, "ior", 1))
	.addTransform(position);

	return obj;
}
