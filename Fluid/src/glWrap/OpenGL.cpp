#include "OpenGL.h"

bool OpenGL::initializeLoader()
{
	auto glewErr = glewInit();

	return glewErr == GLEW_OK;
}