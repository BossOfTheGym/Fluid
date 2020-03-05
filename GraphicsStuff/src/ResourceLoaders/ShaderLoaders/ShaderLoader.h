#pragma once

#include <Common.h>

#include <glWrap/OpenGL.h>

namespace gl
{
	class Shader;
}

namespace res
{
	class SimpleShaderLoader
	{
	public:
		SimpleShaderLoader() = default;

		SimpleShaderLoader(const SimpleShaderLoader&) = default;
		SimpleShaderLoader(SimpleShaderLoader&&)      = default;

		~SimpleShaderLoader() = default;

		SimpleShaderLoader& operator = (const SimpleShaderLoader&) = default;
		SimpleShaderLoader& operator = (SimpleShaderLoader&&)      = default;


	public:
		gl::Shader loadShader(gl::ShaderType type, const String& location);


	private:
		String m_buffer;
	};
}
