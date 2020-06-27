#pragma once
#include "rayTracer.h"
#include "scene.h"
#include "bmpWriter.h"
#include "SceneRenderer.h"
#include "mixerGUI.h"
#include "window.h"
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

	MixerEngine(int width, int height):scene(&window), window(width, height, "Mixer"), viewportRenderer(&window, &scene), gui(&scene)
	{
		gui.onStart(&window);
	}

	void mainLoop();

	void onUpdate();

	~MixerEngine()
	{
		scene.clear();
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
