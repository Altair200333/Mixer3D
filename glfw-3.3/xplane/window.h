#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


#include "input.h"

class Window final
{
public:
	GLFWwindow* window;
	int width;
	int height;
	Input input;
	
	double getTime() const;
	Window(size_t _width, size_t _height, const std::string& name);
	bool shouldClose() const;
	void pollEvents();
	void swapBuffers() const;
	~Window();
	
protected:
	
	static GLFWwindow* createWindow(size_t width, size_t height, const std::string& name);

	static void initGlfw();

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	
	void setCallbacks();

	void onKey(int key, int scancode, int actions, int mods);
	void onMouse(double x, double y);
};
