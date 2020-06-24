#pragma once
#include <glm/glm/glm.hpp>

#include "component.h"

class PointLight: public Component
{
public:
	const static uint64_t componentID = 4;

	glm::vec3 color;
	float intensity;
	PointLight(Object* _owner, glm::vec3 _color, float _intens):Component(_owner), color(_color), intensity(_intens){}
	
	virtual ~PointLight() = default;
};
