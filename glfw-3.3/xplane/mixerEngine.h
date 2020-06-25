#pragma once
#include "rayTracer.h"
#include "scene.h"
#include "window.h"
#include "bmpWriter.h"
#include "SceneRenderer.h"
#include "transform.h"
#include "nlohmann/json.hpp"
#include "mixerGUI.h"
#include "objectBuilder.h"
//This is supposed to be the Core of program, this is where it starts and ends
//On start is setups window and load scene with specific parameters
//after that user calls mainloop in which all input is handled

class MixerEngine
{
public:
	Window window;

	Scene scene;
	SceneRenderer viewportRenderer;
	MixerGUI gui;

	MixerEngine(int width, int height):scene(width, height), window(width, height, "Mixer"), viewportRenderer(&window, &scene), gui(&scene)
	{
		gui.onStart(&window);
	}

	void mainLoop()
	{
		while (!glfwWindowShouldClose(window.window))
		{
			calcDeltaTime(window.getTime());
			viewportRenderer.clearBuffer();
			viewportRenderer.drawScene();
			gui.draw();
			viewportRenderer.swapBuffers();
			window.pollEvents();
			onUpdate();
		}
	}

	void onUpdate()
	{	
		if (window.input.getKeyUp(KeyCode::Z))
		{
			std::cout << "Z\n";

			GLint polygonMode;
			glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
			if (polygonMode == GL_FILL)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			
		}
		if (window.input.getKeyUp(KeyCode::R))
		{
			
			std::cout << "render\n";
			RayTracerEngine rt;
			Bitmap img = rt.render(scene);
			BMPWriter bmpw;
			bmpw.save(img, "render.bmp");
			std::cout << "finished render\n";
			img.clear();
			
		}
		if (window.input.getKeyUp(KeyCode::ESC))
			glfwSetWindowShouldClose(window.window, true);
		auto camera = scene.getActiveCamera();
		if (window.input.getKey(KeyCode::W))
			camera->ProcessKeyboard(FORWARD, deltaTime);
		if (window.input.getKey(KeyCode::S))
			camera->ProcessKeyboard(BACKWARD, deltaTime);
		if (window.input.getKey(KeyCode::A))
			camera->ProcessKeyboard(LEFT, deltaTime);
		if (window.input.getKey(KeyCode::D))
			camera->ProcessKeyboard(RIGHT, deltaTime);
		if (window.input.getKey(KeyCode::E))
			camera->ProcessKeyboard(UP, deltaTime);
		if (window.input.getKey(KeyCode::Q))
			camera->ProcessKeyboard(DOWN, deltaTime);

		if (window.input.getMouseButton(KeyCode::MMB))
		{
			camera->ProcessMouseMovement(window.input.dx(), window.input.dy());
		}

	}

	void loadSceneFromJson(std::string name)
	{
		using json = nlohmann::json;

		std::ifstream i(name);
		json j;
		i >> j;

		BMPWriter bmpw;
		if(j.find("maxBounces") != j.end())
			scene.maxBounces = j.at("maxBounces");
		if (j.find("env") != j.end())
			scene.environment = bmpw.loadJPG(j.at("env"));
		if (j.find("objects") != j.end())
			for (auto& [key, value] : j.at("objects").items())
			{
				auto obj = new Object();
				glm::vec3 color = { value.at("color")[0],value.at("color")[1] ,value.at("color")[2] };
				glm::vec3 position = { value.at("position")[0],value.at("position")[1] ,value.at("position")[2] };
				scene.AddObject(ObjectBuilder().addMesh(value.at("name")).addRenderer(&window).addMaterial(color, value.at("roughness")).addTransform(position));
			}
		if (j.find("lights") != j.end())
			for (auto& [key, value] : j.at("lights").items())
			{
				auto obj = new Object();
				glm::vec3 color = { value.at("color")[0],value.at("color")[1] ,value.at("color")[2] };
				glm::vec3 position = { value.at("position")[0],value.at("position")[1] ,value.at("position")[2]};
				
				obj = ObjectBuilder().addMesh("icosphere.stl").addRenderer(&window).addMaterial(color, 1);
				obj->addComponent(new Transform(obj, position))->addComponent(new PointLight(obj, color, value.at("intensity")));
				scene.lights.push_back(obj);
			}
		for (auto& [key, value] : j.at("cameras").items())
		{
			auto obj = new Object();
			glm::vec3 position = { value.at("position")[0],value.at("position")[1] ,value.at("position")[2] };
			obj->addComponent(new Transform(obj, position))->addComponent(new Camera(obj, static_cast<float>(scene.width) / scene.height, value.at("fov")));
			scene.cameras.push_back(obj);
		}

	}
protected:
	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	void calcDeltaTime(double currentFrame)
	{
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}

};
