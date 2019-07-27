#include <iostream>
#include <filesystem>

#include <glWrap/OpenGL.h>
#include <Window/Window.h>
#include <ResourceLoaders/ShaderLoader.h>

namespace fs = std::filesystem;


int main()
{
	Window::init(100, 100, "window");
	OpenGL::initializeLoader();

	SimpleShaderLoader ssl;
	Shader test = ssl.loadShader(ShaderType::Compute, "assets/shaders/poissonSolver.comp");
	if(test.compiled())
	{ 
		std::cout << "Compiled" << std::endl;
	}
	else
	{
		std::cout << test.infoLog() << std::endl;
	}

	std::cin.get();

	return 0;
}