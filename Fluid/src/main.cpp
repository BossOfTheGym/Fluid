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
	glfwMakeContextCurrent(window);

	glViewport(0, 0, 300, 300);
	glClearColor(1.0, 0.0, 0.0, 1.0);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
	}
}

int main()
{
	Window::init(300, 300, "window");
	OpenGL::initializeLoader();

	mainloop();

	Window::terminate();


	std::cout << "Execution finished" << std::endl;
	std::cin.get();

	return 0;
}