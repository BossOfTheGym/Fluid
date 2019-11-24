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


	//TEST
	/*GLuint vao;
	GLuint vbo;

	GLfloat vertices[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};

	glGenVertexArrays(1, &vao);

	glCreateBuffers(1, &vbo);
	glNamedBufferData(vbo, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 0, 0);
	glBindVertexArray(0);

	voxelSize = 0.5f;*/
	//

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

		//TEST
		/*glBindVertexArray(vao);
		glDrawArrays(GL_POINTS, 0, 2);*/
		//TEST

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
		, Window::Hint{GLFW_CONTEXT_VERSION_MINOR, 6}
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
	//test1();

	//test2();

	test3();

	//testV1();
	return 0;
}





//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow *window);
//
//// settings
//const unsigned int SCR_WIDTH = 800;
//const unsigned int SCR_HEIGHT = 600;
//
//const char *vertexShaderSource = "#version 330 core\n"
//"layout (location = 0) in vec3 aPos;\n"
//"void main()\n"
//"{\n"
//"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
//"}\0";
//const char *fragmentShader1Source = "#version 330 core\n"
//"out vec4 FragColor;\n"
//"void main()\n"
//"{\n"
//"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
//"}\n\0";
//const char *fragmentShader2Source = "#version 330 core\n"
//"out vec4 FragColor;\n"
//"void main()\n"
//"{\n"
//"   FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
//"}\n\0";
//
//
//int main()
//{
//	// glfw: initialize and configure
//	// ------------------------------
//	glfwInit();
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//	#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
//	#endif
//
//														 // glfw window creation
//														 // --------------------
//	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
//	if (window == NULL)
//	{
//		std::cout << "Failed to create GLFW window" << std::endl;
//		glfwTerminate();
//		return -1;
//	}
//	glfwMakeContextCurrent(window);
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//
//	// glad: load all OpenGL function pointers
//	// ---------------------------------------
//	glewInit();
//
//	// build and compile our shader program
//	// ------------------------------------
//	// we skipped compile log checks this time for readability (if you do encounter issues, add the compile-checks! see previous code samples)
//	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	unsigned int fragmentShaderOrange = glCreateShader(GL_FRAGMENT_SHADER); // the first fragment shader that outputs the color orange
//	unsigned int fragmentShaderYellow = glCreateShader(GL_FRAGMENT_SHADER); // the second fragment shader that outputs the color yellow
//	unsigned int shaderProgramOrange = glCreateProgram();
//	unsigned int shaderProgramYellow = glCreateProgram(); // the second shader program
//	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
//	glCompileShader(vertexShader);
//	glShaderSource(fragmentShaderOrange, 1, &fragmentShader1Source, NULL);
//	glCompileShader(fragmentShaderOrange);
//	glShaderSource(fragmentShaderYellow, 1, &fragmentShader2Source, NULL);
//	glCompileShader(fragmentShaderYellow);
//	// link the first program object
//	glAttachShader(shaderProgramOrange, vertexShader);
//	glAttachShader(shaderProgramOrange, fragmentShaderOrange);
//	glLinkProgram(shaderProgramOrange);
//	// then link the second program object using a different fragment shader (but same vertex shader)
//	// this is perfectly allowed since the inputs and outputs of both the vertex and fragment shaders are equally matched.
//	glAttachShader(shaderProgramYellow, vertexShader);
//	glAttachShader(shaderProgramYellow, fragmentShaderYellow);
//	glLinkProgram(shaderProgramYellow);
//
//	// set up vertex data (and buffer(s)) and configure vertex attributes
//	// ------------------------------------------------------------------
//	float firstTriangle[] = {
//		-0.9f, -0.5f, 0.0f,  // left 
//		-0.0f, -0.5f, 0.0f,  // right
//		-0.45f, 0.5f, 0.0f,  // top 
//	};
//	float secondTriangle[] = {
//		0.0f, -0.5f, 0.0f,  // left
//		0.9f, -0.5f, 0.0f,  // right
//		0.45f, 0.5f, 0.0f   // top 
//	};
//	unsigned int VBOs[2], VAOs[2];
//	glGenVertexArrays(2, VAOs); // we can also generate multiple VAOs or buffers at the same time
//	glGenBuffers(2, VBOs);
//	// first triangle setup
//	// --------------------
//	glBindVertexArray(VAOs[0]);
//	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);	// Vertex attributes stay the same
//	glEnableVertexAttribArray(0);
//	// glBindVertexArray(0); // no need to unbind at all as we directly bind a different VAO the next few lines
//	// second triangle setup
//	// ---------------------
//	glBindVertexArray(VAOs[1]);	// note that we bind to a different VAO now
//	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);	// and a different VBO
//	glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
//	glEnableVertexAttribArray(0);
//	// glBindVertexArray(0); // not really necessary as well, but beware of calls that could affect VAOs while this one is bound (like binding element buffer objects, or enabling/disabling vertex attributes)
//
//
//	// uncomment this call to draw in wireframe polygons.
//	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//	// render loop
//	// -----------
//	while (!glfwWindowShouldClose(window))
//	{
//		// input
//		// -----
//		processInput(window);
//
//		// render
//		// ------
//		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//
//		// now when we draw the triangle we first use the vertex and orange fragment shader from the first program
//		glUseProgram(shaderProgramOrange);
//		// draw the first triangle using the data from our first VAO
//		glBindVertexArray(VAOs[0]);
//		glDrawArrays(GL_TRIANGLES, 0, 3);	// this call should output an orange triangle
//											// then we draw the second triangle using the data from the second VAO
//											// when we draw the second triangle we want to use a different shader program so we switch to the shader program with our yellow fragment shader.
//		glUseProgram(shaderProgramYellow);
//		glBindVertexArray(VAOs[1]);
//		glDrawArrays(GL_TRIANGLES, 0, 3);	// this call should output a yellow triangle
//
//											// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//											// -------------------------------------------------------------------------------
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	// optional: de-allocate all resources once they've outlived their purpose:
//	// ------------------------------------------------------------------------
//	glDeleteVertexArrays(2, VAOs);
//	glDeleteBuffers(2, VBOs);
//
//	// glfw: terminate, clearing all previously allocated GLFW resources.
//	// ------------------------------------------------------------------
//	glfwTerminate();
//	return 0;
//}
//
//// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//// ---------------------------------------------------------------------------------------------------------
//void processInput(GLFWwindow *window)
//{
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//}
//
//// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//// ---------------------------------------------------------------------------------------------
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//	// make sure the viewport matches the new window dimensions; note that width and 
//	// height will be significantly larger than specified on retina displays.
//	glViewport(0, 0, width, height);
//}