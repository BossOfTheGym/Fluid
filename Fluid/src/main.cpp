#include <iostream>
#include <filesystem>


#include <Common.h>

#include <glWrap/OpenGL.h>
#include <Window/Window.h>

#include <ResourceLoaders/ShaderLoaders/ShaderLoader.h>
#include <ResourceLoaders/ShapeBuilders/QuadBuilder.h>
#include <ResourceLoaders/TextureBuilders/Texture2D_Builder.h>
#include <ResourceLoaders/ShaderProgramBuilders/ShaderProgramBuilder.h>

namespace fs = std::filesystem;


Texture testTexture(Texture2D_Builder& builder)
{
	using Data   = Texture::Image2D_Data;
	using Params = Texture2D_Builder::SamplingParameters;

	Data textureData = {
		TextureTarget::Texture2D
		, 0
		, InternalFormat::RGBA
		, 400
		, 400
		, PixelDataFormat::RGBA
		, DataType::Float
		, nullptr
	};

	std::vector<float> pixelData(400 * 400 * 4);

	auto iter = pixelData.begin();
	for (int i = 0; i < 400; i++)
	{
		for (int j = 0; j < 400; j++)
		{
			*iter = 1.0f; ++iter;
			*iter = 1.0f; ++iter;
			*iter =	1.0f; ++iter;
			*iter = 1.0f; ++iter;
		}
	}

	textureData.pixels = pixelData.data();

	return builder.buildTexture(textureData, Params{});
}

void mainloop()
{
	//context
	GLFWwindow* window = Window::getContext()->window();

	//set-ups
	glfwShowWindow(window);
	glfwMakeContextCurrent(window);

	glViewport(0, 0, 300, 300);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	//builders
	SimpleShaderLoader   shaderLoader;
	ShaderProgramBuilder shaderProgramBuilder;
	Texture2D_Builder    textureBuilder;
	QuadBuilder          quadBuilder;

	//resources
	VertexArray quad = quadBuilder.buildShape();

	Texture texture = testTexture(textureBuilder);

	Shader vert = shaderLoader.loadShader(ShaderType::Vertex  , "assets/shaders/quad.vert");
	Shader frag = shaderLoader.loadShader(ShaderType::Fragment, "assets/shaders/quad.frag");
	ShaderProgram quadProgram = shaderProgramBuilder.buildProgram(vert, frag);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		quadProgram.use();

		texture.active(static_cast<GLenum>(TextureUnitBase::Texture0));
		texture.bind(TextureTarget::Texture2D);

		quad.bind();
		quad.drawArrays();

		glfwSwapBuffers(window);
	}
}

int main()
{
	Window::init(
		Window::CreationInfo{
			  1200, 600, "window"
			, Window::Hints{
				  Window::Hint{GLFW_DOUBLEBUFFER, GLFW_TRUE}
				, Window::Hint{GLFW_DEPTH_BITS, 32}
				, Window::Hint{GLFW_STENCIL_BITS, 8}
				, Window::Hint{GLFW_CONTEXT_VERSION_MAJOR, 4}
				, Window::Hint{GLFW_CONTEXT_VERSION_MINOR, 5}
				, Window::Hint{GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE}
				, Window::Hint{GLFW_VISIBLE, GL_FALSE}
				, Window::Hint{GLFW_RESIZABLE, GL_FALSE}
		    }
		}
	);

	OpenGL::initializeLoader();

	mainloop();

	Window::terminate();


	std::cout << "Execution finished" << std::endl;

	return 0;
}