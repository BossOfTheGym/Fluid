#include <iostream>
#include <filesystem>

#include <glWrap/OpenGL.h>
#include <Window/Window.h>
#include <ResourceLoaders/ShaderLoaders/ShaderLoader.h>

namespace fs = std::filesystem;


void mainloop()
{
	GLFWwindow* window = Window::getContext()->window();

	glfwShowWindow(window);
	while (glfwWindowShouldClose(window))
	{
		glfwPollEvents();



		glfwSwapBuffers(window);
	}
}

int main()
{
	Window::init(100, 100, "window");
	OpenGL::initializeLoader();

	mainloop();

	std::cin.get();

	return 0;
}