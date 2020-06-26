#pragma once

#include "shader.h"

class Material : public Component
{
public:
	const static uint64_t componentID = 3;
	
	glm::vec3 diffuseColor;

	float roughness;
	Material():Material(nullptr){}
	Material(Object* _owner):Component(_owner)
	{
		diffuseColor = { 1.0f, 0.5f, 0.31f };
		roughness = 0.9f;
	}
	
	Material(Object* _owner, glm::vec3 color, float _rough) :Component(_owner), diffuseColor(color), roughness(_rough)
	{}
	void loadToShader(const Shader& sh) const
	{
		sh.setVec3("diffuse", diffuseColor);
		//sh.setFloat("material.shininess", roughness);
	}

};
