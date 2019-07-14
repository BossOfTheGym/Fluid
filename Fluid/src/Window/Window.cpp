#include "Context.h"


Window::Window()
	: mWindow(nullptr)
{}

GLFWwindow* Window::window() const
{
	return mWindow;
}

//statics
Window* Window::sContext = nullptr;


bool Window::init(int width, int height, const String& name)
{
	auto glfwErr = glfwInit();

	if (glfwErr != GLFW_TRUE)
	{
		return false;
	}

	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	Window::sContext = new Window();
	Window::sContext->mWindow = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

	glfwMakeContextCurrent(Window::sContext->mWindow);

	return true;
}

void Window::terminate()
{
	glfwDestroyWindow(Window::sContext->mWindow);
	glfwTerminate();
}

Window* Window::getContext()
{
	return Window::sContext;
}