#include "MeshRenderer.h"

#include "Loger.h"
#include "mesh.h"
#include "transform.h"

MeshRenderer::MeshRenderer(Window* _window, Object* _owner) :Component(_owner), window(_window)
{
	initMeshRenderer();
}
void MeshRenderer::render(Camera* camera) const
{
	shader.use();
	shader.setVec3("viewPos", camera->owner->getComponent<Transform>()->position);

	// material properties
	shader.setVec3("diffuse", owner->getComponent<Material>()->diffuseColor);

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
void MeshRenderer::initMeshRenderer()
{
	Mesh* mesh = owner->getComponent<Mesh>();
	if(mesh == nullptr)
	{
		Logger::log("Mesh component required");
		return;
	}
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