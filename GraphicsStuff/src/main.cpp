#include <iostream>
#include <filesystem>


#include <Common.h>

#include <Math/MathLib.h>

#include <glWrap/OpenGL.h>
#include <glWrap/Shader/Shader.h>
#include <glWrap/Shader/ShaderProgram.h>
#include <glWrap/Framebuffer/Framebuffer.h>

#include <Window/Window.h>

#include <ResourceLoaders/ShaderLoaders/ShaderLoader.h>
#include <ResourceLoaders/ShapeBuilders/QuadBuilder.h>
#include <ResourceLoaders/TextureBuilders/Texture2D_Builder.h>
#include <ResourceLoaders/ShaderProgramBuilders/ShaderProgramBuilder.h>
#include <ResourceLoaders/FramebufferBuilders/SimpleFramebufferBuilder.h>
#include <ResourceLoaders/ShapeBuilders/BoxBuilder.h>

#include <Miscellanious/SomeTextureBuilders.h>
#include <Miscellanious/PingPongBufer.h>

#include <Miscellanious/ArrowHead.h>

#include <Mesh/Profile.h>

#include <Visual/OctreeVisualization.h>

#include "voxelizer.h"


using math::Float;
using math::Float;
using math::Float;
using math::Float;
using math::Int32;
using math::Vec3;
using math::Mat4;

using math::operator "" _FL;


auto testProfileMesh()
{
	const int COUNT = 10;

	mesh::Profile profile;

	profile.args.reserve(COUNT);
	profile.vals.reserve(COUNT);
	profile.count = COUNT;

	Float h = 2.0_FL / (COUNT + 1);
	for (int i = 1; i <= COUNT; i++)
	{
		auto arg = -1.0_FL + i * h;
		auto val = 1.0_FL - arg * arg;

		profile.args.push_back(arg);
		profile.vals.push_back(val);
	}

	return mesh::constructMeshFromProfile(profile, -1.0_FL, +1.0_FL, 75);
}

auto testSVO()
{
	return vis::fromSVO(
		voxel::svoVoxelize<Int32>(
			testProfileMesh()
			, 16
		)
	);
}

auto testFVO()
{
	return vis::fromFVO
	(
		voxel::fvoVoxelize<Int32>
		(
			testProfileMesh()
			, 128
			, Vec3(0.0_FL)
			, -1, -2, -3
		)
		, [] (const auto& value)
		{
			return value >= 0;
		}
	);
}


void serializeMesh(const mesh::IndicesMesh& mesh)
{
	OFStream output("mesh.vtk");

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

auto voxelizeIndicesMesh(const mesh::IndicesMesh& mesh)
{
	return vis::fromFVO(
		voxel::fvoVoxelize<Int32>(
			mesh
			, 50
			, Vec3(0.0_FL, 0.0_FL, 0.0_FL)
			, -1, -2, -3
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
	res::SimpleShaderLoader shaderLoader;
	res::ShaderProgramBuilder programBuilder;
	res::BoxBuilder boxBuilder;
	misc::ArrowHeadMeshBuilder arrowHeadMeshBuilder;
	misc::ArrowHeadArrayBuilder arrowHeadArrayBuilder(arrowHeadMeshBuilder.indicesMesh());

	serializeMesh(arrowHeadMeshBuilder.indicesMesh());

	//resources
	gl::Shader voxelVert = shaderLoader.loadShader(gl::ShaderType::Vertex  , "assets/shaders/voxel/voxel.vert");
	gl::Shader voxelGeom = shaderLoader.loadShader(gl::ShaderType::Geometry, "assets/shaders/voxel/voxel.geom");
	gl::Shader voxelFrag = shaderLoader.loadShader(gl::ShaderType::Fragment, "assets/shaders/voxel/voxel.frag");
	gl::ShaderProgram voxelProgram = programBuilder.buildProgram(voxelVert, voxelGeom, voxelFrag);

	gl::Shader boxVert = shaderLoader.loadShader(gl::ShaderType::Vertex  , "assets/shaders/box/box.vert");
	gl::Shader boxFrag = shaderLoader.loadShader(gl::ShaderType::Fragment, "assets/shaders/box/box.frag");
	gl::ShaderProgram boxProgram = programBuilder.buildProgram(boxVert, boxFrag);

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

	auto [voxelArray, voxelData, voxelSize] = testSVO();
	auto [vaArrow, vdArrow, vsArrow] = voxelizeIndicesMesh(arrowHeadMeshBuilder.indicesMesh());
	
	gl::Framebuffer defaultFB = gl::Framebuffer::default();


	//view params
	Vec3 viewPos = Vec3(0.0_FL, 4.0_FL, 4.0_FL);
	Vec3 viewPosUpdated = viewPos;

	Mat4 p = glm::perspective(glm::radians(30.0_FL), 1.0_FL * info.width / info.height, 0.5_FL, 100.0_FL);
	Mat4 v = glm::lookAt(viewPos, Vec3(0.0_FL), Vec3(0.0_FL, 1.0_FL, 0.0_FL));
	Mat4 m = Mat4(1.0_FL);
	Mat4 pv = p * v;
	Mat4 vm = v * m;
	Mat4 pvm = p * vm;
	Mat4 r = glm::rotate(Mat4(1.0_FL), glm::radians(0.25_FL), Vec3(0.0_FL, 1.0_FL, 0.0_FL));

	//TEST
	auto fbb = res::SimpleFramebufferBuilder(
		  misc::colorTextureBuilder(512, 512)
		, misc::depthTextureBuilder(512, 512)
	);
	for(int i = 0; i < 1000; i++)
	{
		//auto r = fbb.buildFramebuffer();
	}
	for(int i = 0; i < 10000; i++)
	{
		auto r = vis::dummy();
	}

	//loop
	//state set-ups
	gl::state::viewport(0, 0, info.width, info.height);
	gl::state::enable(gl::Capability::DepthTest);
	gl::state::enable(gl::Capability::CullFace);
	gl::state::cullFace(gl::Face::Back);
	std::cout << "---Mainloop---" << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		//events
		glfwPollEvents();

		//clear
		defaultFB.clearColor(0.05_FL, 0.05_FL, 0.05_FL, 1.0_FL);
		defaultFB.clearDepth(1.0_FL);
		defaultFB.clear(gl::ClearMask::ColorDepth);

		//update
		auto t = glfwGetTime();
		auto cost = static_cast<float>(std::cos(t));
		auto sint = static_cast<float>(std::sin(t));

		//viewPosUpdated = Vec3(4.0_FL * cost, 6.0_FL + 4.0_FL * sint, 4.0_FL * sint);
		v = glm::lookAt(viewPosUpdated, Vec3(0.0_FL), Vec3(0.0_FL, 1.0_FL, 0.0_FL));

		m = r * m;
		pv = p * v;
		vm = v * m;
		pvm = p * vm;

		// render
		gl::state::polygonMode(gl::Face::FrontAndBack, gl::PolygonMode::Line);
		boxProgram.use();
		boxProgram.setUniformMat4(boxPvmLoc, pvm);

		boxArray.bind();
		boxArray.draw();	
		boxArray.unbind();

		boxProgram.unbind();

		gl::state::polygonMode(gl::Face::FrontAndBack, gl::PolygonMode::Fill);
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
	glfwHideWindow(window);
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

	gl::state::initializeLoader();

	test3MainLoop();
	std::cout << "Execution finished" << std::endl;
	std::cin.get();

	Window::terminate();

}


int main()
{
	test3();

	return 0;
}