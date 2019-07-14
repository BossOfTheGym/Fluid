#ifndef SHADERLOADER_H
#define SHADERLOADER_h

#include <Common.h>
#include <glWrap/Shader/Shader.h>

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
	Shader loadShader(ShaderType type, const String& location);


private:
	String m_buffer;
};

#endif