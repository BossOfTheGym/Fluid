#include "Context.h"


Context::Context()
	: mWindow(nullptr)
{}

GLFWwindow* Context::window() const
{
	return mWindow;
}

//statics
Context* Context::sContext = nullptr;


bool Context::init(int width, int height, const String& name)
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
	//glfwWindowHint(GLFW_DECORATED, GL_FALSE);
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	Context::sContext = new Context();
	Context::sContext->mWindow = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

	glfwMakeContextCurrent(Context::sContext->mWindow);


	auto glewErr = glewInit();

	if (glewErr != GLEW_OK)
	{
		return false;
	}


	return true;
}

void Context::terminate()
{
	glfwDestroyWindow(Context::sContext->mWindow);
	glfwTerminate();
}

Context* Context::getContext()
{
	return Context::sContext;
}