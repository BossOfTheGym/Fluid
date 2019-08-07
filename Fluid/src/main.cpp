#include <iostream>
#include <filesystem>


#include <Common.h>

#include <glWrap/OpenGL.h>
#include <glWrap/Framebuffer/Framebuffer.h>

#include <Window/Window.h>

#include <ResourceLoaders/ShaderLoaders/ShaderLoader.h>
#include <ResourceLoaders/ShapeBuilders/QuadBuilder.h>
#include <ResourceLoaders/TextureBuilders/Texture2D_Builder.h>
#include <ResourceLoaders/ShaderProgramBuilders/ShaderProgramBuilder.h>
#include <ResourceLoaders/FramebufferBuilders/SimpleFramebufferBuilder.h>


Texture testTexture(Texture2D_Builder& builder, int width, int height)
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
			*iter = static_cast<float>((i / 10 % 2 == (j / 10 % 2))); ++iter;
			*iter = 0.5f; ++iter;
			*iter =	1.0f; ++iter;
			*iter = 1.0f; ++iter;
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

Texture colorAttachment(Texture2D_Builder& builder, int width, int height)
{
	return builder.buildTexture(
		Texture::Image2D_Data{
			  TextureTarget::Texture2D
			, 0
			, InternalFormat::RGBA
			, width
			, height
			, PixelDataFormat::RGBA
			, DataType::Float
			, nullptr
		}
		, Texture2D_Builder::SamplingParameters{
			  TextureParameterValue::Linear
			, TextureParameterValue::Linear
			, TextureParameterValue::ClampToEdge
		    , TextureParameterValue::ClampToEdge
		}
	);
}

Texture depthAttachment(Texture2D_Builder& builder, int width, int height)
{
	return builder.buildTexture(
		Texture::Image2D_Data{
			  TextureTarget::Texture2D
			, 0
			, InternalFormat::DepthComponent
			, width
			, height
			, PixelDataFormat::DepthComponent
			, DataType::UnsignedByte
			, nullptr
		}
		, Texture2D_Builder::SamplingParameters{}
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


	//builders
	SimpleShaderLoader       shaderLoader;
	ShaderProgramBuilder     shaderProgramBuilder;
	Texture2D_Builder        textureBuilder;
	QuadBuilder              quadBuilder;
	SimpleFramebufferBuilder framebufferBuilder;


	//resources
	const TextureUnit TEST_TEXTURE = 0;

	VertexArray quad = quadBuilder.buildShape();

	Texture texture = testTexture(textureBuilder, info.width, info.height);

	Shader vert = shaderLoader.loadShader(ShaderType::Vertex  , "assets/shaders/quad.vert");
	Shader frag = shaderLoader.loadShader(ShaderType::Fragment, "assets/shaders/quad.frag");
	ShaderProgram quadProgram = shaderProgramBuilder.buildProgram(vert, frag);

	Texture color = colorAttachment(textureBuilder, info.width, info.height);
	Texture depth = depthAttachment(textureBuilder, info.width, info.height);
	Framebuffer testBuffer = framebufferBuilder.buildFramebuffer(color, depth);

	Framebuffer defaultBuffer = Framebuffer::default();

	//loop
	//state set-ups
	OpenGL::viewport(0, 0, info.width, info.height);

	testBuffer.bindFramebuffer(FramebufferTarget::Framebuffer);
	testBuffer.clearColor(1.0f, 0.0f, 0.0f, 1.0f);
	testBuffer.clear(ClearMask::Color);

	defaultBuffer.bindFramebuffer(FramebufferTarget::Framebuffer);
	defaultBuffer.clearColor(0.0f, 0.0f, 0.0f, 1.0f);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		defaultBuffer.clear(ClearMask::Color);

		quadProgram.use();

		texture.bindToUnit(TEST_TEXTURE);

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