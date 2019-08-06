#include "OpenGL.h"

bool OpenGL::initializeLoader()
{
	auto glewErr = glewInit();

	return glewErr == GLEW_OK;
}

//state management
void OpenGL::blendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	glBlendColor(red, green, blue, alpha);
}

void OpenGL::blendEquation(BlendMode mode)
{
	glBlendEquation(static_cast<GLenum>(mode));
}

void OpenGL::blendEquationi(BufferNumber number, BlendMode mode)
{
	glBlendEquationi(number, static_cast<GLenum>(mode));
}

void OpenGL::blendEquationSeparate(BlendMode rgb, BlendMode alpha)
{
	glBlendEquationSeparate(static_cast<GLenum>(rgb), static_cast<GLenum>(alpha));
}

void OpenGL::blendEquationSeparatei(BufferNumber number, BlendMode rgb, BlendMode alpha)
{
	glBlendEquationSeparatei(number, static_cast<GLenum>(rgb), static_cast<GLenum>(alpha));
}

void OpenGL::blendFunc(BlendFactor srcFactor, BlendFactor dstFactor)
{
	glBlendFunc(static_cast<GLenum>(srcFactor), static_cast<GLenum>(dstFactor));
}

void OpenGL::blendFunci(BufferNumber number, BlendFactor srcFactor, BlendFactor dstFactor)
{
	glBlendFunci(number, static_cast<GLenum>(srcFactor), static_cast<GLenum>(dstFactor));
}

void OpenGL::blendFuncSeparate(BlendFactor srcRGB, BlendFactor dstRGB, BlendFactor srcAlpha, BlendFactor dstAlpha)
{
	glBlendFuncSeparate(
		  static_cast<GLenum>(srcRGB)
		, static_cast<GLenum>(dstRGB)
		, static_cast<GLenum>(srcAlpha)
		, static_cast<GLenum>(dstAlpha)
	);
}

void OpenGL::blendFuncSeparatei(BufferNumber number, BlendFactor srcRGB, BlendFactor dstRGB, BlendFactor srcAlpha, BlendFactor dstAlpha)
{
	glBlendFuncSeparatei(
		  number
		, static_cast<GLenum>(srcRGB)
		, static_cast<GLenum>(dstRGB)
		, static_cast<GLenum>(srcAlpha)
		, static_cast<GLenum>(dstAlpha)
	);
}


void OpenGL::cullFace(Face mode)
{
	glCullFace(static_cast<GLenum>(mode));
}


void OpenGL::depthFunc(DepthFunc func)
{
	glDepthFunc(static_cast<GLenum>(func));
}


void OpenGL::enable(Capability parameter)
{
	glEnable(static_cast<GLenum>(parameter));
}

void OpenGL::enablei(Capability parameter, CapabalityIndex index)
{
	glEnablei(static_cast<GLenum>(parameter), index);
}

void OpenGL::disable(Capability parameter)
{
	glDisable(static_cast<GLenum>(parameter));
}

void OpenGL::disablei(Capability parameter, CapabalityIndex index)
{
	glDisablei(static_cast<GLenum>(parameter), index);
}


void frontFace(FrontFace mode)
{
	glFrontFace(static_cast<GLenum>(mode));
}


void polygonMode(Face face, PolygonMode mode)
{
	glPolygonMode(static_cast<GLenum>(face), static_cast<GLenum>(mode));
}


void viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	glViewport(x, y, width, height);
}