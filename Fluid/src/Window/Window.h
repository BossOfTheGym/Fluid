#ifndef WINDOW_H
#define WINDOW_H

#include <Common.h>

#include <GLFW/glfw3.h>


class Window
{
public:
	struct Hint
	{
		int hint;
		int value;
	};

	using Hints = std::vector<Hint>;

	struct CreationInfo
	{
		int width;
		int height;
		String name;
		Hints hints;
	};

public:
	static bool init(const CreationInfo& info);

	static Window* getContext();

	static void terminate();


private:
	static Window* sContext;


private:
	Window();


public:
	GLFWwindow* window() const;

	
private:
	GLFWwindow* mWindow;
};

#endif