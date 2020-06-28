#pragma once
#include <glm/glm/glm.hpp>

#include "component.h"

class PointLight final: public Component
{
public:
	const static uint64_t componentID = 5;

	glm::vec3 color;
	float intensity;
	
	PointLight(Object* _owner):PointLight(_owner, {1,1,1}, 1){}
	
	PointLight(Object* _owner, glm::vec3 _color, float _intens):Component(_owner), color(_color), intensity(_intens){}
};
