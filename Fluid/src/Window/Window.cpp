#include "Window.h"


Window::Window()
	: m_window(nullptr)
{}

GLFWwindow* Window::window() const
{
	return m_window;
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
	Window::sContext->m_info   = std::move(info);
	Window::sContext->m_window = glfwCreateWindow(info.width, info.height, info.name.c_str(), nullptr, nullptr);

	glfwMakeContextCurrent(Window::sContext->m_window);

	return true;
}

void Window::terminate()
{
	glfwDestroyWindow(Window::sContext->m_window);
	glfwTerminate();

	delete(Window::sContext);
	sContext = nullptr;
}

Window* Window::getContext()
{
	return Window::sContext;
}

const Window::CreationInfo& Window::info() const
{
	return m_info;
}