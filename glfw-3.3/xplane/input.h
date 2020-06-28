#pragma once
#include <GLFW/glfw3.h>
#include <map>

struct KeyCode final
{
	static const int W = GLFW_KEY_W, A = GLFW_KEY_A,
		S = GLFW_KEY_S, D = GLFW_KEY_D, Q = GLFW_KEY_Q, E = GLFW_KEY_E,
		R = GLFW_KEY_R, Z = GLFW_KEY_Z, ESC = GLFW_KEY_ESCAPE, MMB = GLFW_MOUSE_BUTTON_MIDDLE;
};
class Input final
{
protected:
	//code - action map
	std::map<int, int> keys;

	double mouseX = 0;
	double mouseY = 0;
	bool firstMouse = true;

	double deltaX;
	double deltaY;
public:
	static const int Up = GLFW_RELEASE;
	static const int Release = GLFW_PRESS;
	static const int Down = GLFW_REPEAT;
	GLFWwindow* _window;

	Input() = default;
	void callback(int key, int scancode, int action, int mods);
	bool getKeyDown(int keyCode);
	bool getKeyUp(int keyCode);
	bool getKey(int keyCode);
	bool getMouseButton(int keyCode);
	void resetKeys();
	double dx() const;
	double dy() const;
	void mouse_callback(double xpos, double ypos);

};
