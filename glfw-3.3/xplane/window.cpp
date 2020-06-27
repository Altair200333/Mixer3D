#include "window.h"

double Window::getTime() const
{
	return glfwGetTime();
}
Window::Window(size_t _width, size_t _height, const std::string& name) :width(_width), height(_height)
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
bool Window::shouldClose() const
{
	return glfwWindowShouldClose(window);
}
void Window::pollEvents()
{
	input.resetKeys();
	glfwPollEvents();
}
void Window::swapBuffers() const
{
	glfwSwapBuffers(window);
}
Window::~Window()
{
	glfwTerminate();
}


GLFWwindow* Window::createWindow(size_t width, size_t height, const std::string& name)
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

void Window::initGlfw()
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

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void Window::setCallbacks()
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
		obj->onKey(a, b, c, d);
	};

	glfwSetKeyCallback(window, func3);

}

void Window::onKey(int key, int scancode, int actions, int mods)
{
	input.callback(key, scancode, actions, mods);
}
void Window::onMouse(double x, double y)
{
	input.mouse_callback(x, y);
}