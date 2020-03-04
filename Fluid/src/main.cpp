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

#include "ArrowHead.h"
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
			, 50
		)
	);
}

auto testFVO()
{
	return vis::fromFVO
	(
		voxel::fvoVoxelize<int>
		(
			testProfileMesh()
			, 50
			, Vec3(0.0f)
		)
		, [] (const auto& value)
		{
			return value >= 0;
		}
	);
}


void serializeMesh(const IndicesMesh& mesh)
{
	std::ofstream output("mesh.vtk");

	output.precision(6);
	output << "# vtk DataFile Version 3.0" << std::endl
		<< "vtk output" << std::endl
		<< "ASCII" << std::endl
		<< "DATASET UNSTRUCTURED_GRID" << std::endl
		<< "POINTS " << mesh.points.size() << " float" << std::endl;
	for (auto& point : mesh.points)
	{
		output << point.x << " " << point.y << " " << point.z << std::endl;
	}

	output << std::endl;

	int triangles = mesh.indices.size() / 3;
	output << "CELLS " << triangles << " " << triangles * 4 << std::endl;
	int i = 0;
	int e = static_cast<int>(mesh.indices.size());
	while (i + 2 < e)
	{
		output << 3 << " "
			<< mesh.indices[i] << " "
			<< mesh.indices[i + 1] << " "
			<< mesh.indices[i + 2] << std::endl;

		i += 3;
	}

	output << std::endl;

	output << "CELL_TYPES " << triangles << std::endl;
	for (int i = 0; i < triangles; i++)
	{
		output << 5 << std::endl;
	}
}

auto voxelizeIndicesMesh(const IndicesMesh& mesh)
{
	return vis::fromFVO(
		voxel::fvoVoxelize<int>(
			mesh
			, 50
			, Vec3(0.0f, 0.0f, 0.0f)
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
	ArrowHeadMeshBuilder arrowHeadMeshBuilder;
	ArrowHeadArrayBuilder arrowHeadArrayBuilder(arrowHeadMeshBuilder.indicesMesh());

	serializeMesh(arrowHeadMeshBuilder.indicesMesh());

	//resources
	Shader voxelVert = shaderLoader.loadShader(ShaderType::Vertex  , "assets/shaders/voxel/voxel.vert");
	Shader voxelGeom = shaderLoader.loadShader(ShaderType::Geometry, "assets/shaders/voxel/voxel.geom");
	Shader voxelFrag = shaderLoader.loadShader(ShaderType::Fragment, "assets/shaders/voxel/voxel.frag");
	ShaderProgram voxelProgram = programBuilder.buildProgram(voxelVert, voxelGeom, voxelFrag);

	Shader boxVert = shaderLoader.loadShader(ShaderType::Vertex  , "assets/shaders/box/box.vert");
	Shader boxFrag = shaderLoader.loadShader(ShaderType::Fragment, "assets/shaders/box/box.frag");
	ShaderProgram boxProgram = programBuilder.buildProgram(boxVert, boxFrag);

	auto boxPvmLoc = boxProgram.getUniformLocation("uPVM");

	auto voxelSizeLoc    = voxelProgram.getUniformLocation("uVoxelSize");
	auto voxelPVMLoc     = voxelProgram.getUniformLocation("uPVM");
	auto voxelPVLoc      = voxelProgram.getUniformLocation("uPV");
	auto voxelVMLoc      = voxelProgram.getUniformLocation("uVM");
	auto voxelPLoc       = voxelProgram.getUniformLocation("uP");
	auto voxelVLoc       = voxelProgram.getUniformLocation("uV");
	auto voxelMLoc       = voxelProgram.getUniformLocation("uM");
	auto voxelViewPos    = voxelProgram.getUniformLocation("uEyePos");

	auto arrowHeadArray = arrowHeadArrayBuilder.buildShape();
	auto boxArray = boxBuilder.buildShape();
	auto [voxelArray, voxelData, voxelSize] = testFVO();
	auto [vaArrow, vdArrow, vsArrow] = voxelizeIndicesMesh(arrowHeadMeshBuilder.indicesMesh());
	
	Framebuffer defaultFB = Framebuffer::default();


	//view params
	Vec3 viewPos = Vec3(0.0f, 8.0f, 4.0f);
	Vec3 viewPosUpdated = viewPos;

	Mat4 p = glm::perspective(glm::radians(30.0f), 1.0f * info.width / info.height, 0.5f, 100.0f);
	Mat4 v = glm::lookAt(viewPos, Vec3(0.0f), Vec3(0.0f, 1.0f, 0.0f));
	Mat4 m = Mat4(1.0f);
	Mat4 pv = p * v;
	Mat4 vm = v * m;
	Mat4 pvm = p * vm;
	Mat4 r = glm::rotate(Mat4(1.0f), glm::radians(0.25f), Vec3(0.0f, 1.0f, 0.0f));

	//loop
	//state set-ups
	OpenGL::viewport(0, 0, info.width, info.height);
	OpenGL::enable(Capability::DepthTest);
	OpenGL::enable(Capability::CullFace);
	OpenGL::cullFace(Face::Back);
	std::cout << "---Mainloop---" << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		//events
		glfwPollEvents();

		//clear
		defaultFB.clearColor(0.05f, 0.05f, 0.05f, 1.0f);
		defaultFB.clearDepth(1.0f);
		defaultFB.clear(ClearMask::ColorDepth);

		//update
		auto t = glfwGetTime();
		auto cost = static_cast<float>(std::cos(t));
		auto sint = static_cast<float>(std::sin(t));

		viewPosUpdated = Vec3(4.0f * cost, 6.0f + 4.0f * sint, 4.0f * sint);
		v = glm::lookAt(viewPosUpdated, Vec3(0.0f), Vec3(0.0f, 1.0f, 0.0f));

		m = r * m;
		pv = p * v;
		vm = v * m;
		pvm = p * vm;

		// render
		OpenGL::polygonMode(Face::FrontAndBack, PolygonMode::Line);
		boxProgram.use();
		boxProgram.setUniformMat4(boxPvmLoc, pvm);

		boxArray.bind();
		boxArray.draw();	
		boxArray.unbind();

		boxProgram.unbind();

		OpenGL::polygonMode(Face::FrontAndBack, PolygonMode::Fill);
		voxelProgram.use();
		voxelProgram.setUniformMat4(voxelPVMLoc, pvm);
		voxelProgram.setUniformMat4(voxelPVLoc, pv);
		voxelProgram.setUniformMat4(voxelVMLoc, vm);
		voxelProgram.setUniformMat4(voxelPLoc, p);
		voxelProgram.setUniformMat4(voxelVLoc, v);
		voxelProgram.setUniformMat4(voxelMLoc, m);
		voxelProgram.setUniformVec3(voxelViewPos, viewPosUpdated);
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
			1400, 1000, "window"
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