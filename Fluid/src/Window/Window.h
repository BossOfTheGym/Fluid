#ifndef WINDOW_H
#define WINDOW_H

#include <Common.h>

#include <GLFW/glfw3.h>


class Window
{
public:
	struct Hint
	{
		int hint  = 0;
		int value = 0;
	};

	using Hints = std::vector<Hint>;

	struct CreationInfo
	{
		int width   = 0;
		int height  = 0;
		String name = "";
		Hints hints = {};
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

	const CreationInfo& info() const;
	
private:
	GLFWwindow* m_window;
	CreationInfo m_info;
};

#endif