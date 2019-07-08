#pragma once

#include <Common.h>

#include <GLFW/glfw3.h>


class Context
{
public:
	static bool init(int width, int height, const String& name);

	static Context* getContext();

	static void terminate();


private:
	static Context* sContext;


private:
	Context();


public:
	GLFWwindow* window() const;

	
public:
	GLFWwindow* mWindow;
};