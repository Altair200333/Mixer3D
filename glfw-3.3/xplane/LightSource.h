#pragma once
#include "shader.h"

class LightSource
{
public:
	virtual ~LightSource() = default;
	virtual void loadToShader(const Shader& shader) = 0;
};
