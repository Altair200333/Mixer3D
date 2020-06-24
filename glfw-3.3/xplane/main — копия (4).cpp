#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "camera.h"
#include "cube.h"
#include "directionalight.h"
#include "mesh.h"
#include "plane.h"
#include "pointlight.h"
#include "scene.h"
#include "shader.h"
#include <glm/glm/glm.hpp>

// settings
const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 900;


// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void calcDeltaTime(float currentFrame)
{
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void addpipe(Scene* scene, float shiftX, float shiftY, float sceneScale)
{
	std::vector<glm::vec3> coords;
	std::ifstream fs;
	fs.open("PetroGM_exp/45_traj_pet_format.las");
	float val;
	while (fs >> val)
	{
		float x, y, z;

		fs >> x;
		fs >> y;
		fs >> z;
		glm::vec3 v = {(x - shiftX) * sceneScale, z * sceneScale, (y - shiftY) * sceneScale};
		coords.push_back(v);
		for (int i = 0; i < 6; i++)
			fs >> val;
	}
	float side = 0.3f;

	Mesh* c2 = new Plane(coords[0] + glm::vec3({ -side / 2, 0, side / 2 }), coords[0] + glm::vec3({ side / 2, 0, side / 2 }),
	                     coords[0] + glm::vec3({ -side / 2, 0, -side / 2 }), coords[0] + glm::vec3({ side / 2, 0, -side / 2 }));
	scene->objects.push_back(c2);
	//24*(coords.size()-1)
	for (int i=0;i<coords.size()-1;i++)
	{
		//6 vert
		Mesh* c = new Plane(coords[i] + glm::vec3({ -side/2, 0, -side / 2 }), coords[i] + glm::vec3({ side / 2, 0, -side / 2 }),
		                    coords[i+1] + glm::vec3({ -side / 2, 0, -side / 2 }), coords[i + 1] + glm::vec3({ side / 2, 0, -side / 2 }));
		scene->objects.push_back(c);

		c = new Plane(coords[i] + glm::vec3({ -side / 2, 0, -side / 2 }), coords[i] + glm::vec3({ -side / 2, 0, side / 2 }),
		              coords[i + 1] + glm::vec3({ -side / 2, 0, -side / 2 }), coords[i + 1] + glm::vec3({ -side / 2, 0, side / 2 }));
		scene->objects.push_back(c);

		c = new Plane(coords[i] + glm::vec3({ side / 2, 0, -side / 2 }), coords[i] + glm::vec3({ side / 2, 0, side / 2 }),
		              coords[i + 1] + glm::vec3({ side / 2, 0, -side / 2 }), coords[i + 1] + glm::vec3({ side / 2, 0, side / 2 }));
		scene->objects.push_back(c);

		c = new Plane(coords[i] + glm::vec3({ -side / 2, 0, side / 2 }), coords[i] + glm::vec3({ side / 2, 0, side / 2 }),
		              coords[i + 1] + glm::vec3({ -side / 2, 0, side / 2 }), coords[i + 1] + glm::vec3({ side / 2, 0, side / 2 }));
		scene->objects.push_back(c);
	}
	c2 = new Plane(coords[coords.size() - 1] + glm::vec3({ -side / 2, 0, side / 2 }), coords[coords.size() - 1] + glm::vec3({ side / 2, 0, side / 2 }),
	               coords[coords.size() - 1] + glm::vec3({ -side / 2, 0, -side / 2 }), coords[coords.size() - 1] + glm::vec3({ side / 2, 0, -side / 2 }));
	scene->objects.push_back(c2);
}

void proceedScene1()
{
	Scene* scene = makeScene(SCR_WIDTH, SCR_HEIGHT);

	PointLight pl({0.2, 0.2, 0.2}, {0.8, 0.8, 0.8}, {3.2f, -1.0f, 2.0f});
	DirectionalLight d({0.2, 0.2, 0.2}, {0.8, 0.8, 0.8}, {-0.5f, -2.0f, -0.5f }, 0.3f);

	Mesh* c = new Cube(0.2f);
	c->model = glm::translate(c->model, {3.2f, -1.0f, 2.0f});
	scene->objects.push_back(c);
	float shiftX = 583699.00993122f;
	float shiftY = 5728762.2398f;

	Plane* p = new Plane({ 1,1,1 }, { 2,1,1 }, { 1,2,2 });
	scene->objects.push_back(p);
	
	float sceneScale = 0.01f;

	addpipe(scene, shiftX, shiftY, sceneScale);

	
	std::vector<LightSource*> light;
	light.push_back(&pl);
	light.push_back(&d);
	
	while (!glfwWindowShouldClose(scene->window))
	{
		float currentFrame = glfwGetTime();
		calcDeltaTime(currentFrame);

		// input
		scene->camera.handleWindowInput(scene->window, deltaTime);

		// render
		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto obj : scene->objects)
		{
			scene->camera.render(obj, light);
		}

		glfwSwapBuffers(scene->window);
		glfwPollEvents();
	}
}

int main()
{
	proceedScene1();

	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}
