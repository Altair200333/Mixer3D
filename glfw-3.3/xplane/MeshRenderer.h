#pragma once

#include "component.h"
#include "LightSource.h"
#include "mesh.h"
#include "transform.h"
#include "window.h"

class MeshRenderer : public Component
{
	const Window* window;
public:
    Shader shader;
	
    glm::mat4 model;
    unsigned int VBO, VAO;

    const static uint64_t componentID = 1;
	MeshRenderer(Window* _window, Object* _owner):Component(_owner), window(_window)
	{
		initMesh();
	}
    void render(Camera* camera, std::vector<LightSource>& ls) const
    {
        shader.use();
        shader.setVec3("viewPos", camera->owner->getComponent<Transform>()->position);
        for (auto& l : ls)
            l.loadToShader(shader);
        // material properties
        owner->getComponent<Material>()->loadToShader(shader);

        // view/projection transformations
        const glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), camera->aspectRatio, 0.1f, 1000.0f);
        const glm::mat4 view = camera->GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // world transformation
        const glm::mat4 worldModel = glm::translate(model, owner->getComponent<Transform>()->position);
        shader.setMat4("model", worldModel);

        // render the cube
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, owner->getComponent<Mesh>()->vertexCount);
    }
	void initMesh()
	{
		Mesh* mesh = owner->getComponent<Mesh>();
		model = glm::mat4(1.0f);
		shader = Shader();

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * owner->getComponent<Mesh>()->vertexCount * 6, &mesh->vertices[0], GL_STATIC_DRAW);

		glBindVertexArray(VAO);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);


		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// note that we update the lamp's position attribute's stride to reflect the updated buffer data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
};
