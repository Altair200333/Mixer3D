#pragma once

#include <glm/glm/glm.hpp>

#include "shader.h"

#include "window.h"
#include "camera.h"

class MeshRenderer final: public Component
{
	const Window* window;
public:
    Shader shader;
	
    glm::mat4 model;
    unsigned int VBO, VAO;
	
    MeshRenderer(Window* _window, Object* _owner);
	void render(Camera* camera) const;
    void initMeshRenderer();
	~MeshRenderer();
};
