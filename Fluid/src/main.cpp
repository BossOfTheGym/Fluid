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


auto testProfileMesh()
{
	const int COUNT = 10;

	Profile<float> profile;

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
	return vis::fromSVO(
		voxel::svoVoxelize<int>(
			testProfileMesh()
			, 30
		)
	);
}

auto testFVO()
{
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
		defaultFB.clearColor(0.5F, 0.5f, 0.5f, 1.0f);
		defaultFB.clearDepth(1.0f);
		defaultFB.clear(ClearMask::ColorDepth);

		//render
		m = r * m;
		auto pvm = p * v * m;


		boxProgram.use();
		boxProgram.setUniformMat4(boxPvmLoc, pvm);

		boxArray.bind();
		boxArray.draw();	
		boxArray.unbind();

		boxProgram.unbind();

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
			    , Window::Hint{GLFW_DEPTH_BITS  , 24}
		        , Window::Hint{GLFW_STENCIL_BITS, 8}
		        , Window::Hint{GLFW_CONTEXT_VERSION_MAJOR, 4}
		        , Window::Hint{GLFW_CONTEXT_VERSION_MINOR, 5}
		        , Window::Hint{GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API}
		        , Window::Hint{GLFW_CLIENT_API, GLFW_OPENGL_API}
		        , Window::Hint{GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE}
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
	test3();

	return 0;
}