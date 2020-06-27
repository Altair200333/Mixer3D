#include "input.h"

void Input::callback(int key, int scancode, int action, int mods)
{
	if (keys.find(key) != keys.end())
	{
		keys[key] = action;
	}
	else
	{
		keys.insert(std::pair(key, action));
	}
}
bool Input::getKeyDown(int keyCode)
{
	return keys.find(keyCode) != keys.end() && keys[keyCode] == Down;
}
bool Input::getKeyUp(int keyCode)
{
	return keys.find(keyCode) != keys.end() && keys[keyCode] == Up;
}
bool Input::getKey(int keyCode)
{
	return keys.find(keyCode) != keys.end() && _window != nullptr && glfwGetKey(_window, keyCode) == GLFW_PRESS;
}
bool Input::getMouseButton(int keyCode)
{
	return glfwGetMouseButton(_window, keyCode);
}
void Input::resetKeys()
{
	for (auto it = keys.begin(); it != keys.end(); ++it)
	{
		it->second = -1;
	}
	deltaY = deltaX = 0;
}
double Input::dx() const
{
	return deltaX;
}
double Input::dy() const
{
	return deltaY;
}
void Input::mouse_callback(double xpos, double ypos)
{
	if (firstMouse)
	{
		mouseX = xpos;
		mouseY = ypos;
		firstMouse = false;
	}

	deltaX = xpos - mouseX;
	deltaY = mouseY - ypos;

	mouseX = xpos;
	mouseY = ypos;
}