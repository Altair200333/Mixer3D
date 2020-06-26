#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <map>
#include <vector>

struct KeyCode
{
	static const int W = GLFW_KEY_W, A = GLFW_KEY_A,
		S = GLFW_KEY_S, D = GLFW_KEY_D, Q = GLFW_KEY_Q, E = GLFW_KEY_E,
		R = GLFW_KEY_R, Z = GLFW_KEY_Z, ESC = GLFW_KEY_ESCAPE, MMB = GLFW_MOUSE_BUTTON_MIDDLE;
};
class Input
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
	void callback(int key, int scancode, int action, int mods)
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
	bool getKeyDown(int keyCode)
	{
		return keys.find(keyCode) != keys.end() && keys[keyCode] == Down;
	}
	bool getKeyUp(int keyCode)
	{
		return keys.find(keyCode) != keys.end() && keys[keyCode] == Up;
	}
	bool getKey(int keyCode)
	{
		return keys.find(keyCode) != keys.end() && _window!=nullptr && glfwGetKey(_window, keyCode) == GLFW_PRESS;
	}
	bool getMouseButton(int keyCode)
	{
		return glfwGetMouseButton(_window, keyCode);
	}
	void resetKeys()
	{
		for(auto it = keys.begin(); it != keys.end(); ++it)
		{
			it->second = -1;
		}
		deltaY = deltaX = 0;
	}
	double dx() const
	{
		return deltaX;
	}
	double dy() const
	{
		return deltaY;
	}
	void mouse_callback(double xpos, double ypos)
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

};

class Window
{
public:
	GLFWwindow* window;
	int width;
	int height;
	Input input;
	
	double getTime() const 
	{
		return glfwGetTime();
	}
	Window(size_t _width, size_t _height, const std::string& name):width(_width), height(_height)
	{
		initGlfw();
		window = createWindow(width, height, name);
		input._window = window;
		// glad: load all OpenGL function pointers	
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
		}

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);


		setCallbacks();
	}

	void pollEvents()
	{
		input.resetKeys();
		glfwPollEvents();
	}
	virtual ~Window()
	{
		glfwTerminate();
	}
protected:
	
	static GLFWwindow* createWindow(size_t width, size_t height, const std::string& name)
	{
		GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
		if (window == nullptr)
		{
			glfwTerminate();
			return nullptr;
		}

		glfwMakeContextCurrent(window);

		// tell GLFW to capture our mouse
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		return window;
	}

	static void initGlfw()
	{
		// glfw: initialize and configure
		// ------------------------------
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	
	void setCallbacks()
	{
		glfwSetWindowUserPointer(window, this);

		auto func1 = [](GLFWwindow* w, double a, double b)
		{
			Window* obj = static_cast<Window*>(glfwGetWindowUserPointer(w));
			obj->onMouse(a, b);
		};
		glfwSetCursorPosCallback(window, func1);

		auto func2 = [](GLFWwindow* w, int nw, int nh)
		{
			static_cast<Window*>(glfwGetWindowUserPointer(w))->framebuffer_size_callback(w, nw, nh);
		};
		glfwSetFramebufferSizeCallback(window, func2);
		
		auto func3 = [](GLFWwindow* w, int a, int b, int c, int d)
		{
			Window* obj = static_cast<Window*>(glfwGetWindowUserPointer(w));
			obj->onKey(a,b,c,d);
		};
		
		glfwSetKeyCallback(window, func3);

	}

	void onKey(int key, int scancode, int actions, int mods)
	{
		input.callback(key, scancode, actions, mods);
	}
	void onMouse(double x, double y)
	{
		input.mouse_callback(x,y);
	}
};
