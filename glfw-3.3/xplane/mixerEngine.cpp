#include "mixerEngine.h"

void MixerEngine::mainLoop()
{
	while (!window.shouldClose())
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

void MixerEngine::onUpdate()
{
	if (window.input.getKeyUp(KeyCode::Z))
	{
		Logger::log("Toggle wireframe mode");

		GLint polygonMode;
		glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
		if (polygonMode == GL_FILL)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}

	if (window.input.getKeyUp(KeyCode::ESC))
		glfwSetWindowShouldClose(window.window, true);
	auto camera = scene.getActiveCamera();

	if (camera == nullptr)
		return;

	if (window.input.getKeyUp(KeyCode::R))
	{
		RayTracerEngine rt;
		Bitmap img = rt.render(scene, window.width, window.height);
		BMPWriter bmpw;
		bmpw.save(img, "render.bmp");
		Logger::log("Render finished");
		img.clear();

	}

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