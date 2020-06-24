#pragma once
#include "LightSource.h"

class PointLight : public LightSource
{
public:
	glm::vec3 ambientColor;
	glm::vec3 color;
	glm::vec3 position;
	float constant = 1.0f;//1.0f, 0.09, 0.032,
	float linear = 0.091f;
	float quadratic = 0.032f;
	
	PointLight(glm::vec3 _ambientColor, glm::vec3 color, glm::vec3 pos)
	{
		this->color = color;
		this->ambientColor = _ambientColor;
		position = pos;
	}

	void loadToShader(const Shader& shader) override
	{
		shader.setVec3("light.position", position);
		shader.setVec3("light.ambient", ambientColor);
		shader.setVec3("light.diffuse", color);
		shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		shader.setFloat("light.constant", constant);
		shader.setFloat("light.linear", linear);
		shader.setFloat("light.quadratic", quadratic);
	}
	
};
