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

#include "Profile.h"
#include "voxelizer.h"
#include "OctreeVisualization.h"


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



auto testProfileMesh()
{
	const int COUNT = 10;

	Profile<float> profile;

	/*profile.args = {-0.75f, -0.25 , 0.0f, 0.25f, 0.75f};
	profile.vals = { 0.5f ,  0.75f, 1.0f, 0.75f, 0.5f};
	profile.count = 5;*/

	profile.args.reserve(COUNT);
	profile.vals.reserve(COUNT);
	profile.count = COUNT;

	float h = 2.0f / (COUNT + 1);
	for (int i = 1; i <= COUNT; i++)
	{
		auto arg = -1.0f + i * h;
		auto val = 1.0f - arg * arg;

		profile.args.push_back(arg);
		profile.vals.push_back(val);
	}

	return constructMeshFromProfile(profile, -1.0f, +1.0f, 75);
}

auto testSVO()
{
	/*
	Mesh{
	{
	{Vec3(-1.0f, -1.0f, +1.0f), Vec3(+1.0f, -1.0f, +1.0f), Vec3(+0.0f, +0.5f, +0.0f)}
	, {Vec3(+1.0f, -1.0f, +1.0f), Vec3(+1.0f, -1.0f, -1.0f), Vec3(+0.0f, +0.5f, +0.0f)}
	, {Vec3(+1.0f, -1.0f, -1.0f), Vec3(-1.0f, -1.0f, -1.0f), Vec3(+0.0f, +0.5f, +0.0f)}
	, {Vec3(-1.0f, -1.0f, -1.0f), Vec3(-1.0f, -1.0f, +1.0f), Vec3(+0.0f, +0.5f, +0.0f)}

	, {Vec3(+1.0f, -1.0f, +1.0f), Vec3(+1.0f, -1.0f, -1.0f), Vec3(-1.0f, -1.0f, -1.0f)}
	, {Vec3(-1.0f, -1.0f, -1.0f), Vec3(-1.0f, -1.0f, +1.0f), Vec3(+1.0f, -1.0f, +1.0f)}
	}
	}
	*/

	return vis::fromSVO(
		voxel::svoVoxelize<int>(
			testProfileMesh()
			, 30
		)
	);
}

auto testFVO()
{
	/*
	Mesh{
	{
	{Vec3(-1.0f, -1.0f, +1.0f), Vec3(+1.0f, -1.0f, +1.0f), Vec3(+0.0f, +1.0f, +0.0f)}
	, {Vec3(+1.0f, -1.0f, +1.0f), Vec3(+1.0f, -1.0f, -1.0f), Vec3(+0.0f, +1.0f, +0.0f)}
	, {Vec3(+1.0f, -1.0f, -1.0f), Vec3(-1.0f, -1.0f, -1.0f), Vec3(+0.0f, +1.0f, +0.0f)}
	, {Vec3(-1.0f, -1.0f, -1.0f), Vec3(-1.0f, -1.0f, +1.0f), Vec3(+0.0f, +1.0f, +0.0f)}

	, {Vec3(+1.0f, -1.0f, +1.0f), Vec3(+1.0f, -1.0f, -1.0f), Vec3(-1.0f, -1.0f, -1.0f)}
	, {Vec3(-1.0f, -1.0f, -1.0f), Vec3(-1.0f, -1.0f, +1.0f), Vec3(+1.0f, -1.0f, +1.0f)}
	}
	}
	*/;

	return vis::fromFVO(
		voxel::fvoVoxelize<int>(
			testProfileMesh()
			, 40
			, Vec3(0.0f)
			)
		, [] (const auto& value)
		{
			return value >= 0;
		}
	);
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
	Shader voxelVert = shaderLoader.loadShader(ShaderType::Vertex  , "assets/shaders/voxel/voxel.vert");
	Shader voxelGeom = shaderLoader.loadShader(ShaderType::Geometry, "assets/shaders/voxel/voxel.geom");
	Shader voxelFrag = shaderLoader.loadShader(ShaderType::Fragment, "assets/shaders/voxel/voxel.frag");
	ShaderProgram voxelProgram = programBuilder.buildProgram(voxelVert, voxelGeom, voxelFrag);

	Shader boxVert = shaderLoader.loadShader(ShaderType::Vertex  , "assets/shaders/box/box.vert");
	Shader boxFrag = shaderLoader.loadShader(ShaderType::Fragment, "assets/shaders/box/box.frag");
	ShaderProgram boxProgram = programBuilder.buildProgram(boxVert, boxFrag);

	auto boxPvmLoc = boxProgram.getUniformLocation("uPVM");
	auto voxelSizeLoc = voxelProgram.getUniformLocation("uVoxelSize");
	auto voxelPvmLoc  = voxelProgram.getUniformLocation("uPVM");

	VertexArray boxArray = boxBuilder.buildShape();
	auto [voxelArray, voxelData, voxelSize] = testFVO();

	Framebuffer defaultFB = Framebuffer::default();


	//view params
	Mat4 p = glm::perspective(glm::radians(45.0f), 1.0f * info.width / info.height, 0.5f, 100.0f);
	Mat4 v = glm::lookAt(Vec3(5.0f, 2.0f, 5.0f), Vec3(0.0f), Vec3(0.0f, 1.0f, 0.0f));
	Mat4 m = Mat4(1.0f);
	Mat4 r = glm::rotate(Mat4(1.0f), glm::radians(0.25f), Vec3(0.0f, 1.0f, 0.0f));


	//loop
	//state set-ups
	OpenGL::viewport(0, 0, info.width, info.height);
	OpenGL::polygonMode(Face::FrontAndBack, PolygonMode::Line);
	OpenGL::enable(Capability::DepthTest);
	OpenGL::enable(Capability::CullFace);
	OpenGL::cullFace(Face::Back);


	std::cout << "---Mainloop---" << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		//events
		glfwPollEvents();

		//clear
		defaultFB.clearColor(0.5f, 0.5f, 0.5f, 1.0f);
		defaultFB.clearDepth(1.0f);
		defaultFB.clear(ClearMask::ColorDepth);

		//render
		m = r * m;
		auto pvm = p * v * m;


		/*boxProgram.use();
		boxProgram.setUniformMat4(boxPvmLoc, pvm);

		boxArray.bind();
		boxArray.draw();	
		boxArray.unbind();

		boxProgram.unbind();*/


		voxelProgram.use();
		voxelProgram.setUniformMat4(voxelPvmLoc, pvm);
		voxelProgram.setUniform1f(voxelSizeLoc, voxelSize);

		voxelArray.bind();
		voxelArray.draw();
		voxelArray.unbind();

		voxelProgram.unbind();

		//swap front/back
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