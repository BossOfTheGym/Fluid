#pragma once

#include <Common.h>

#include <GLFW/glfw3.h>


class Window
{
public:
	static bool init(int width, int height, const String& name);

	static Window* getContext();

	static void terminate();


private:
	static Window* sContext;


private:
	Window();


public:
	GLFWwindow* window() const;

	
public:
	GLFWwindow* mWindow;
};