#pragma once
#include "rayTracer.h"
#include "scene.h"
#include "window.h"
#include "bmpWriter.h"
#include "SceneRenderer.h"
#include "transform.h"
#include "nlohmann/json.hpp"

//This is supposed to be the Core of program, this is where it starts and ends
//On start is setups window and load scene with specific parameters
//after that user calls mainloop in which all input is handled
class ObjectBuilder final
{
	Object* obj;
public:
	ObjectBuilder() { obj = new Object(); }
	
	ObjectBuilder& addMesh(std::string name)
	{
		obj->addComponent<Mesh>(new Mesh(name, obj));
		return *this;
	}
	ObjectBuilder& addRenderer(Window* window)
	{
		obj->addComponent<MeshRenderer>(new MeshRenderer(window, obj));

		return *this;
	}
	ObjectBuilder& addMaterial()
	{
		obj->addComponent<Material>(new Material(obj));

		return *this;
	}
	ObjectBuilder& addMaterial(glm::vec3 color, float _rough)
	{
		obj->addComponent<Material>(new Material(obj, color, _rough));

		return *this;
	}
	ObjectBuilder& addTransform()
	{
		addTransform({0,0,0});
		return *this;
	}
	ObjectBuilder& addTransform(glm::vec3 position)
	{
		obj->addComponent<Transform>(new Transform(obj, position));

		return *this;
	}
	operator Object*() const
	{
		return obj;
	}
};
class MixerEngine
{
public:
	Window window;

	Scene scene;
	SceneRenderer viewportRenderer;
	
	MixerEngine(int width, int height):scene(width, height), window(width, height, "Mixer"), viewportRenderer(&window)
	{
		
	}

	void mainLoop()
	{
		while (!glfwWindowShouldClose(window.window))
		{
			calcDeltaTime(window.getTime());
			viewportRenderer.drawScene(scene);
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

		
		scene.maxBounces = j.at("maxBounces");
		for (auto& [key, value] : j.at("objects").items())
		{
			auto obj = new Object();
			glm::vec3 color = { value.at("color")[0],value.at("color")[1] ,value.at("color")[2] };
			glm::vec3 position = { value.at("position")[0],value.at("position")[1] ,value.at("position")[2] };
			scene.AddObject(ObjectBuilder().addMesh(value.at("name")).addRenderer(&window).addMaterial(color, value.at("roughness")).addTransform(position));
		}
		for (auto& [key, value] : j.at("lights").items())
		{
			auto obj = new Object();
			glm::vec3 color = { value.at("color")[0],value.at("color")[1] ,value.at("color")[2] };
			glm::vec3 position = { value.at("position")[0],value.at("position")[1] ,value.at("position")[2]};
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
