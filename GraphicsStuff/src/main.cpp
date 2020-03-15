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
#include <ResourceLoaders/FramebufferBuilders/NColorFramebufferBuilder.h>
#include <ResourceLoaders/FramebufferBuilders/SimpleFramebufferBuilder.h>
#include <ResourceLoaders/FramebufferBuilders/ColorBufferBuilder.h>
#include <ResourceLoaders/ShapeBuilders/BoxBuilder.h>

#include <Miscellanious/SomeTextureBuilders.h>
#include <Miscellanious/PingPongBufer.h>
#include <Miscellanious/ArrowHead.h>

#include <Mesh/Profile.h>

#include <Visual/GridArrayBuilder.h>

#include <DataStructures/Grid.h>

#include "voxelizer.h"


using math::Float;
using math::Float;
using math::Float;
using math::Float;
using math::Int32;
using math::Vec3;
using math::Mat4;
using math::Vec3i32;

using math::operator "" _FL;


auto testProfileMesh()
{
	const int COUNT = 50;

	mesh::Profile profile;

	profile.args.reserve(COUNT);
	profile.vals.reserve(COUNT);
	profile.count = COUNT;

	Float h = 2.0_FL / (COUNT + 1);
	for (int i = 1; i <= COUNT; i++)
	{
		auto arg = -1.0_FL + i * h;
		auto val = std::abs(0.5 * std::sin(3 * arg)) + 0.2;

		profile.args.push_back(arg);
		profile.vals.push_back(val);
	}

	return mesh::constructMeshFromProfile(profile, -1.0_FL, +1.0_FL, 250);
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


const int WIDTH  = 1500;
const int HEIGHT = 1000;

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
	res::QuadBuilder quadBuilder;
	res::ColorBufferBuilder f16FramebufferBuilder
	(
		  misc::rgbaf16TextureBuilder(WIDTH, HEIGHT)		
	);
	res::NColorFBBuilder nColorFBBuilder
	(
		  misc::rgbaf16TextureBuilder(WIDTH, HEIGHT)
		, misc::depthTextureBuilder(WIDTH, HEIGHT)
		, res::value<2>{}
	);


	// TEST
	using Vi   = math::Vec3i32;
	using AABB = primitive::AABB;

	mesh::TriangleIndicesMesh mmmm
	(
		{
			  Vec3{+1.0_FL, +1.0_FL, +1.0_FL}
			, Vec3{+1.0_FL, +1.0_FL, -1.0_FL}
			, Vec3{-1.0_FL, -1.0_FL, -1.0_FL}

			, Vec3{+1.0_FL, +1.0_FL, +1.0_FL}
			, Vec3{+1.0_FL, -1.0_FL, +1.0_FL}
			, Vec3{-1.0_FL, +1.0_FL, +1.0_FL}
		}
		, {3, 4, 5, 0, 1, 2}
		, Mat4{1.0f}
	);
	auto tm = testProfileMesh().triangles;
	ds::FullGrid<int> g(AABB{Vec3{-1.0_FL}, Vec3{+1.0_FL}}, Vec3i32{192});
	
	voxel::meshToCells(tm, g);
	auto [cellsArray, cellsBuffer, cellSize] = vis::buildCellData(g);
	// END TEST


	// shaders and programs
	gl::Shader quadVert = shaderLoader.loadShader(gl::ShaderType::Vertex  , "assets/shaders/quad.vert");
	gl::Shader quadFrag = shaderLoader.loadShader(gl::ShaderType::Fragment, "assets/shaders/quad.frag");

	gl::Shader voxelVert = shaderLoader.loadShader(gl::ShaderType::Vertex  , "assets/shaders/voxel.vert");
	gl::Shader voxelGeom = shaderLoader.loadShader(gl::ShaderType::Geometry, "assets/shaders/voxel.geom");
	gl::Shader voxelFrag = shaderLoader.loadShader(gl::ShaderType::Fragment, "assets/shaders/voxel.frag");

	gl::Shader boxVert = shaderLoader.loadShader(gl::ShaderType::Vertex  , "assets/shaders/box.vert");
	gl::Shader boxFrag = shaderLoader.loadShader(gl::ShaderType::Fragment, "assets/shaders/box.frag");

	gl::Shader gaussFrag    = shaderLoader.loadShader(gl::ShaderType::Fragment, "assets/shaders/gauss.frag");
	gl::Shader mixFrag      = shaderLoader.loadShader(gl::ShaderType::Fragment, "assets/shaders/mix.frag");
	gl::Shader hdrGammaFrag = shaderLoader.loadShader(gl::ShaderType::Fragment, "assets/shaders/hdr_gamma.frag");

	gl::ShaderProgram boxProgram      = programBuilder.buildProgram(boxVert, boxFrag);
	gl::ShaderProgram voxelProgram    = programBuilder.buildProgram(voxelVert, voxelGeom, voxelFrag);
	gl::ShaderProgram quadProgram     = programBuilder.buildProgram(quadVert, quadFrag);
	gl::ShaderProgram blurProgram     = programBuilder.buildProgram(quadVert, gaussFrag);
	gl::ShaderProgram mixProgram      = programBuilder.buildProgram(quadVert, mixFrag);
	gl::ShaderProgram hdrGammaProgram = programBuilder.buildProgram(quadVert, hdrGammaFrag);

	// framebuffers
	gl::Framebuffer defaultFB = gl::Framebuffer::default();
	
	const int COLOR_TEXTURE = 0;
	const int BLOOM_TEXTURE = 1;
	auto [offScreen, offColors, offDepth] = nColorFBBuilder.buildFramebuffer();
	auto [mixed, mixedColor] = f16FramebufferBuilder.buildFramebuffer();
	misc::PingPongBuffer gaussFramebuffers
	(
		  f16FramebufferBuilder.buildFramebuffer()
		, f16FramebufferBuilder.buildFramebuffer()
	);

	// shader locations & binding points
	const gl::TextureUnit QUAD_FRAG_IMAGE = 0;

	const gl::TextureUnit MIX_FRAG_IMAGE1 = 0;
	const gl::TextureUnit MIX_FRAG_IMAGE2 = 1;

	const gl::TextureUnit HDR_GAMMA_FRAG_IMAGE = 0;

	const gl::TextureUnit GAUSS_FRAG_IMAGE = 0;

	const Int32 HORIZONTAL_PASS = 0;
	const Int32 VERTICAL_PASS   = 1;

	auto boxPvmLoc = boxProgram.getUniformLocation("uPVM");

	auto voxelSizeLoc = voxelProgram.getUniformLocation("uVoxelSize");
	auto voxelPVMLoc  = voxelProgram.getUniformLocation("uPVM");
	auto voxelPVLoc   = voxelProgram.getUniformLocation("uPV");
	auto voxelVMLoc   = voxelProgram.getUniformLocation("uVM");
	auto voxelPLoc    = voxelProgram.getUniformLocation("uP");
	auto voxelVLoc    = voxelProgram.getUniformLocation("uV");
	auto voxelMLoc    = voxelProgram.getUniformLocation("uM");
	auto voxelViewPos = voxelProgram.getUniformLocation("uEyePos");

	auto gaussPassLoc = blurProgram.getUniformLocation("uPass");

	// vertex arrays
	auto boxArray  = boxBuilder.buildShape();
	auto quadArray = quadBuilder.buildShape();
	//auto [voxelArray, voxelData, voxelSize] = testSVO();
	
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

	// loop
	while (!glfwWindowShouldClose(window))
	{
		//events
		glfwPollEvents();

		//update
		auto t = glfwGetTime() / 2;
		auto cost = static_cast<float>(std::cos(t));
		auto sint = static_cast<float>(std::sin(t));

		viewPosUpdated = Vec3(3.0_FL, 2.0_FL, 3.0_FL);
		v = glm::lookAt(viewPosUpdated, Vec3(0.0_FL), Vec3(0.0_FL, 1.0_FL, 0.0_FL));

		m = r * m;
		pv = p * v;
		vm = v * m;
		pvm = p * vm;

		// 1. frags & bloom
		{
			gl::state::viewport(0, 0, info.width, info.height);
			gl::state::enable(gl::Capability::DepthTest);
			gl::state::enable(gl::Capability::CullFace);
			gl::state::cullFace(gl::Face::Back);
			
			offScreen.bindFramebuffer(gl::FramebufferTarget::DrawFramebuffer);
			offScreen.clearColor(0.0_FL, 0.0_FL, 0.0_FL, 1.0_FL);
			offScreen.clearDepth(1.0_FL);
			offScreen.clear(gl::ClearMask::ColorDepth);

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
			voxelProgram.setUniformVec3(voxelSizeLoc, cellSize);

			cellsArray.bind();
			cellsArray.draw();
			cellsArray.unbind();

			voxelProgram.unbind();
		}

		// 2. gaussian blur
		{
			auto& [frame0, color0] = gaussFramebuffers.previous();
			auto& [frame1, color1] = gaussFramebuffers.current();

			// copy bloom output to gauss framebuffer
			frame1.bindFramebuffer(gl::FramebufferTarget::DrawFramebuffer);

			quadProgram.use();
			offColors[BLOOM_TEXTURE].bindToUnit(QUAD_FRAG_IMAGE);

			quadArray.bind();
			quadArray.draw();
			//quadArray.unbind();

			//quadProgram.unbind();

			// blur
			blurProgram.use();

			//quadArray.bind();
			auto& [curr, currColor] = gaussFramebuffers.current();
			auto& [prev, prevColor] = gaussFramebuffers.previous();
			for (int k = 0; k < 2; k++)
			{
				blurProgram.setUniform1i(gaussPassLoc, HORIZONTAL_PASS);
				currColor.bindToUnit(GAUSS_FRAG_IMAGE);							
				prev.bindFramebuffer(gl::FramebufferTarget::DrawFramebuffer);
				quadArray.draw();

				blurProgram.setUniform1i(gaussPassLoc, VERTICAL_PASS);
				prevColor.bindToUnit(GAUSS_FRAG_IMAGE);			
				curr.bindFramebuffer(gl::FramebufferTarget::DrawFramebuffer);
				quadArray.draw();
			}
			//quadArray.unbind();

			//blurProgram.unbind();
		}

		// 3. combine frag & bloom
		{
			mixed.bindFramebuffer(gl::FramebufferTarget::DrawFramebuffer);

			auto& image1 = gaussFramebuffers.current().color;
			auto& image2 = offColors[COLOR_TEXTURE];

			mixProgram.use();

			image1.bindToUnit(MIX_FRAG_IMAGE1);
			image2.bindToUnit(MIX_FRAG_IMAGE2);

			//quadArray.bind();
			quadArray.draw();
			//quadArray.unbind();

			//mixProgram.unbind();
		}

		// 4. tone mapping & gamma correction
		{
			gl::state::disable(gl::Capability::DepthTest);

			defaultFB.bindFramebuffer(gl::FramebufferTarget::DrawFramebuffer);			

			hdrGammaProgram.use();
			mixedColor.bindToUnit(HDR_GAMMA_FRAG_IMAGE);

			//quadArray.bind();
			quadArray.draw();
			//quadArray.unbind();

			//hdrGammaProgram.unbind();
		}

		//swap front/back
		glfwSwapBuffers(window);
	}
	glfwHideWindow(window);
}

void test3()
{
	Window::init(
		Window::CreationInfo{
			WIDTH, HEIGHT, "window"
			, Window::Hints{
			      Window::Hint{GLFW_DOUBLEBUFFER, GLFW_TRUE}
			    , Window::Hint{GLFW_DEPTH_BITS  , 24}
		        , Window::Hint{GLFW_STENCIL_BITS, 8}
		        , Window::Hint{GLFW_CONTEXT_VERSION_MAJOR, 4}
		        , Window::Hint{GLFW_CONTEXT_VERSION_MINOR, 5}
		        , Window::Hint{GLFW_VISIBLE  , GL_FALSE}
		        , Window::Hint{GLFW_RESIZABLE, GL_FALSE}
		    }
		}
	);

	gl::state::initializeLoader();

	test3MainLoop();
	std::cout << "Execution finished. Press any key..." << std::endl;
	std::cin.get();

	Window::terminate();

}


int main()
{
	test3();
	
	return 0;
}