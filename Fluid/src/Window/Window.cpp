#include "Window.h"


Window::Window()
	: mWindow(nullptr)
{}

GLFWwindow* Window::window() const
{
	return mWindow;
}

//statics
Window* Window::sContext = nullptr;


bool Window::init(const CreationInfo& info)
{
	auto glfwErr = glfwInit();

	if (glfwErr != GLFW_TRUE)
	{
		return false;
	}

	for (auto&[hint, value] : info.hints)
	{
		glfwWindowHint(hint, value);
	}

	Window::sContext = new Window();
	Window::sContext->mWindow = glfwCreateWindow(info.width, info.height, info.name.c_str(), nullptr, nullptr);

	glfwMakeContextCurrent(Window::sContext->mWindow);

	return true;
}

void Window::terminate()
{
	glfwDestroyWindow(Window::sContext->mWindow);
	glfwTerminate();

	delete(Window::sContext);
	sContext = nullptr;
}

Window* Window::getContext()
{
	return Window::sContext;
}