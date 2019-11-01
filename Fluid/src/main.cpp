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
#include <ResourceLoaders/ShapeBuilders/BoxBuilder.h>

#include <misc/SomeTextureBuilders.h>
#include <misc/PingPongBufer.h>
#include <misc/memory/SimpleLinearAllocator.h>

#include <voxelizer.h>


void test1Mainloop()
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

void test1()
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

	test1Mainloop();

	Window::terminate();

	std::cout << "Execution finished" << std::endl;
}


void test2()
{
	using SLA = misc::mem::SimpleLinearAllocator<int>;

	SLA linearPool(100);

	std::nounitbuf(std::cout);
	for (int i = 0; i < 100; i++)
	{
		decltype(auto) token = linearPool.alloc(i);
		decltype(auto) data  = linearPool.get(token);

		std::cout << data << std::endl;
	}
	std::cout.flush();

	linearPool.clear();
}


void test3MainLoop()
{
	//context
	decltype(auto) context = Window::getContext();
	decltype(auto) window  = context->window();
	decltype(auto) info    = context->info();

	//set-ups
	glfwShowWindow(window);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	//loader & builders
	SimpleShaderLoader shaderLoader;
	ShaderProgramBuilder programBuilder;
	BoxBuilder boxBuilder;

	//resources
	std::cout << "---Initializing voxel shaders----" << std::endl;
	Shader voxelVert = shaderLoader.loadShader(ShaderType::Vertex  , "assets/shaders/voxel/voxel.vert");
	Shader voxelGeom = shaderLoader.loadShader(ShaderType::Geometry, "assets/shaders/voxel/voxel.geom");
	Shader voxelFrag = shaderLoader.loadShader(ShaderType::Fragment, "assets/shaders/voxel/voxel.frag");

	std::cout << "---Building voxel program---" << std::endl;
	ShaderProgram voxelProgram = programBuilder.buildProgram(voxelVert, voxelGeom, voxelFrag);

	std::cout << "---Getting uniform locations from voxel program---" << std::endl;
	auto voxelSizeLoc = voxelProgram.getUniformLocation("uVoxelSize");
	auto voxelPvmLoc  = voxelProgram.getUniformLocation("uPVM");

	std::cout << "---Initializing box shaders---" << std::endl;
	Shader boxVert = shaderLoader.loadShader(ShaderType::Vertex  , "assets/shaders/box/box.vert");
	Shader boxFrag = shaderLoader.loadShader(ShaderType::Fragment, "assets/shaders/box/box.frag");

	std::cout << "---Building box program---" << std::endl;
	ShaderProgram boxProgram = programBuilder.buildProgram(boxVert, boxFrag);

	std::cout << "---Getting uniforms from box program---" << std::endl;
	auto boxPvmLoc = boxProgram.getUniformLocation("uPVM");

	std::cout << "---Building shapes & voxel model---" << std::endl;
	VertexArray boxArray = boxBuilder.buildShape();
	auto [voxelsArray, voxelsBuffer, voxelSize] = voxelizeV1(16);

	std::cout << "---Get default framebuffer---" << std::endl;
	Framebuffer defaultFB = Framebuffer::default();

	std::cout << "---Initializing view parameters---" << std::endl;
	Mat4 p = glm::perspective(glm::radians(45.0f), 1.0f * info.width / info.height, 0.5f, 100.0f);
	Mat4 v = glm::lookAt(Vec3(5.0f, 1.0f, 5.0f), Vec3(0.0f), Vec3(0.0f, 1.0f, 0.0f));
	Mat4 m = Mat4(1.0f);
	Mat4 r = glm::rotate(Mat4(1.0f), glm::radians(0.3f), Vec3(0.0f, 1.0f, 0.0f));

	//loop
	//state set-ups
	std::cout << "---Setting up OpenGL states---" << std::endl;
	OpenGL::viewport(0, 0, info.width, info.height);
	OpenGL::polygonMode(Face::FrontAndBack, PolygonMode::Line);
	OpenGL::enable(Capability::DepthTest);
	OpenGL::disable(Capability::CullFace);

	std::cout << "---Mainloop---" << std::endl;
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window))
	{
		//events
		std::cout << "---Poll events---" << std::endl;
		glfwPollEvents();

		//clear
		std::cout << "---Clear default framebuffer---" << std::endl;
		defaultFB.clearColor(0.5f, 0.5f, 0.5f, 1.0f);
		defaultFB.clearDepth(1.0f);
		defaultFB.clear(ClearMask::ColorDepth);

		//render
		std::cout << "---Update view parameters---" << std::endl;
		m = r * m;
		auto pvm = p * v * m;


		std::cout << "---Bind box program---" << std::endl;
		boxProgram.use();

		std::cout << "---Set view parameters---" << std::endl;
		boxProgram.setUniformMat4(boxPvmLoc, pvm);

		std::cout << "---Bind box array---" << std::endl;
		boxArray.bind();

		std::cout << "---Draw box array---" << std::endl;
		boxArray.draw();

		std::cout << "---Unbind box array---" << std::endl;
		boxArray.unbind();

		std::cout << "---Unbind box program---" << std::endl;
		boxProgram.unbind();


		std::cout << "---Bind voxel program---" << std::endl;
		voxelProgram.use();

		std::cout << "---Set view and voxel parameters---" << std::endl;
		voxelProgram.setUniform1f(voxelSizeLoc, voxelSize);
		voxelProgram.setUniformMat4(voxelPvmLoc, pvm);

		std::cout << "---Bind voxel array---" << std::endl;
		voxelsArray.bind();

		std::cout << "---Draw voxel array---" << std::endl;
		voxelsArray.draw();

		std::cout << "---Unbind voxel array---" << std::endl;
		voxelsArray.unbind();

		std::cout << "---Unbind voxel program---" << std::endl;
		voxelProgram.unbind();

		//swap front/back
		std::cout << "---Swap buffers---" << std::endl;
		glfwSwapBuffers(window);
	}
}

void test3()
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

	test3MainLoop();

	Window::terminate();

	std::cout << "Execution finished" << std::endl;
}


int main()
{
	//test1();

	//test2();

	test3();

	//testV1();
	return 0;
}