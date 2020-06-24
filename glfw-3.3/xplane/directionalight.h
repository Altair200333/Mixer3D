#pragma once
#include <glm/glm/glm.hpp>

#include "LightSource.h"

class DirectionalLight: public LightSource
{
public:
	glm::vec3 ambientColor;
	glm::vec3 difuse;
	glm::vec3 specular;
	glm::vec3 direction;
	float strength;

	DirectionalLight(glm::vec3 _ambientColor, glm::vec3 color, glm::vec3 dir, float str)
	{
		this->difuse = color;
		this->ambientColor = _ambientColor;
		this->specular = {1,1,1 };
		strength = str;
		
		direction = dir;
	}
	void loadToShader(const Shader& shader) override
	{
		shader.setVec3("dirLight.direction", direction);
		shader.setVec3("dirLight.ambient", ambientColor);
		shader.setVec3("dirLight.diffuse", difuse);
		shader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("dirLight.strength", strength);
	}

};
