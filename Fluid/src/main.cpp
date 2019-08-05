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


Texture testTexture(Texture2D_Builder& builder, int width, int height, const Vec4& initColor = Vec4(0.0f, 0.0f, 0.0f, 1.0f))
{
	using Data   = Texture::Image2D_Data;
	using Params = Texture2D_Builder::SamplingParameters;

	const int CHANNELS = 4;


	Data textureData = {
		  TextureTarget::Texture2D
		, 0
		, InternalFormat::RGBA
		, width
		, height
		, PixelDataFormat::RGBA
		, DataType::Float
		, nullptr
	};

	std::vector<float> pixelData(static_cast<size_t>(width) * height * CHANNELS);

	auto iter = pixelData.begin();
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			*iter = initColor.r; ++iter;
			*iter = initColor.g; ++iter;
			*iter =	initColor.b; ++iter;
			*iter = initColor.a; ++iter;
		}
	}

	textureData.pixels = pixelData.data();

	return builder.buildTexture(
		textureData
		, Params{
			  TextureParameterValue::Linear
			, TextureParameterValue::Linear
		    , TextureParameterValue::ClampToEdge
		    , TextureParameterValue::ClampToEdge
		}
	);
}

void mainloop()
{
	//context
	decltype(auto) context = Window::getContext();
	decltype(auto) window  = context->window();
	decltype(auto) info    = context->info();

	//set-ups
	glfwShowWindow(window);
	glfwMakeContextCurrent(window);

	glViewport(0, 0, info.width, info.height);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	//builders
	SimpleShaderLoader   shaderLoader;
	ShaderProgramBuilder shaderProgramBuilder;
	Texture2D_Builder    textureBuilder;
	QuadBuilder          quadBuilder;

	//resources
	VertexArray quad = quadBuilder.buildShape();

	Texture texture = testTexture(textureBuilder, info.width, info.height);

	Shader vert = shaderLoader.loadShader(ShaderType::Vertex  , "assets/shaders/quad.vert");
	Shader frag = shaderLoader.loadShader(ShaderType::Fragment, "assets/shaders/quad.frag");
	ShaderProgram quadProgram = shaderProgramBuilder.buildProgram(vert, frag);

	//loop
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
				, Window::Hint{GLFW_DEPTH_BITS  , 32}
				, Window::Hint{GLFW_STENCIL_BITS, 8}
				, Window::Hint{GLFW_CONTEXT_VERSION_MAJOR, 4}
				, Window::Hint{GLFW_CONTEXT_VERSION_MINOR, 5}
				, Window::Hint{GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE}
				, Window::Hint{GLFW_VISIBLE  , GL_FALSE}
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