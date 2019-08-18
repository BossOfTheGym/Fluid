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

#include <misc/SomeTextureBuilders.h>
#include <misc/PingPongBufer.h>


void mainloop()
{
	//context
	decltype(auto) context = Window::getContext();
	decltype(auto) window  = context->window();
	decltype(auto) info    = context->info();


	//set-ups
	glfwShowWindow(window);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);


	//builders
	SimpleShaderLoader   shaderLoader;
	ShaderProgramBuilder shaderProgramBuilder;

	QuadBuilder quadBuilder;

	SimpleFramebufferBuilder framebufferBuilder(
		  misc::checkedTextureBuilder(info.width, info.height)
		, misc::depthTextureBuilder(info.width, info.height)
	);


	//resources
	const TextureUnit TEST_TEXTURE = 0;

	const TextureUnit IMAGE = 0;
	const TextureUnit FIELD = 1;

	VertexArray quad = quadBuilder.buildShape();

	Shader quadVert  = shaderLoader.loadShader(ShaderType::Vertex  , "assets/shaders/quad.vert");
	Shader quadFrag  = shaderLoader.loadShader(ShaderType::Fragment, "assets/shaders/quad.frag");
	Shader frameFrag = shaderLoader.loadShader(ShaderType::Fragment, "assets/shaders/field.frag");

	ShaderProgram quadProgram  = shaderProgramBuilder.buildProgram(quadVert, quadFrag);
	ShaderProgram frameProgram = shaderProgramBuilder.buildProgram(quadVert, frameFrag);

	Framebuffer defaultBuffer = Framebuffer::default();

	PingPongBuffer pingPongBuffer(
		  framebufferBuilder.buildFramebuffer()
		, framebufferBuilder.buildFramebuffer()
	);


	//loop
	//state set-ups
	OpenGL::viewport(0, 0, info.width, info.height);

	while (!glfwWindowShouldClose(window))
	{
		//events
		glfwPollEvents();

		//swap 
		pingPongBuffer.swap();

		//set-ups
		auto& [currFrame, currColor, currDepth] = pingPongBuffer.current();
		auto& [prevFrame, prevColor, prevDepth] = pingPongBuffer.previous();

		//field update
		currFrame.bindFramebuffer(FramebufferTarget::Framebuffer);
		currFrame.clearColor(1.0f, 1.0f, 1.0f, 1.0f);
		currFrame.clearDepth(1.0f);
		currFrame.clear(ClearMask::ColorDepth);

		frameProgram.use();

		prevColor.bindToUnit(IMAGE);

		quad.bind();
		quad.drawArrays();

		//final render
		defaultBuffer.bindFramebuffer(FramebufferTarget::Framebuffer);
		defaultBuffer.clearColor(1.0f, 1.0f, 1.0f, 1.0f);
		defaultBuffer.clearDepth(1.0f);
		defaultBuffer.clear(ClearMask::ColorDepth);

		quadProgram.use();

		currColor.bindToUnit(TEST_TEXTURE);

		quad.bind();
		quad.drawArrays();

		//swap front/back
		glfwSwapBuffers(window);
	}
}


int main()
{
	Window::init(
		Window::CreationInfo{
			  1024, 1024, "window"
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