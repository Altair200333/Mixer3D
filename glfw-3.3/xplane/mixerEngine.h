#pragma once
#include "rayTracer.h"
#include "scene.h"
#include "window.h"
#include "bmpWriter.h"
#include "SceneRenderer.h"
#include "transform.h"

//This is supposed to be the Core of program, this is where it starts and ends
//On start is setups window and load scene with specific parameters
//after that user calls mainloop in which all input is handled
class ObjectBuilder final
{
	Object* obj;
public:
	ObjectBuilder() { obj = new Object(); }
	
	ObjectBuilder& loadMesh(std::string name)
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
		//scene.AddObject(ObjectBuilder().loadMesh("uv.stl").addRenderer(&window).addMaterial());
		//scene.AddObject(ObjectBuilder().loadMesh("pl.stl").addRenderer(&window).addMaterial().addTransform());
		scene.AddObject(ObjectBuilder().loadMesh("tst.stl").addRenderer(&window).addMaterial().addTransform({1,0.2f,1}));
		//scene.AddObject(ObjectBuilder().loadMesh("suz.stl").addRenderer(&window).addMaterial().addTransform({1,0.2f,1}));
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
