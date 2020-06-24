#pragma once
#include <glm/glm/glm.hpp>

#include "component.h"

class Transform: public Component
{
public:
	const static uint64_t componentID = 1;
	Transform(Object* _owner) :Transform(_owner, {0,0,0}) {}
	Transform(Object* _owner, glm::vec3 _position) :Component(_owner), position(_position) {}

	void translate(glm::vec3 shift)
	{
		position += shift;
	}
	glm::vec3 position;
};
