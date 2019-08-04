#include "ShaderLoader.h"

Shader SimpleShaderLoader::loadShader(ShaderType type, const String& location)
{
	Shader shader;


	IFStream inputFile(location);

	m_buffer.clear();
	m_buffer.assign(
		  std::istreambuf_iterator<char>(inputFile)
		, std::istreambuf_iterator<char>()
	);

	shader.createShader(type);
	shader.shaderSource(m_buffer);
	shader.compileShader();
	if (shader.compiled())
	{
		std::cout << "Compiled shader: " << location << std::endl;
	}
	else
	{
		std::cout << "Failed to compile shader. Error: " << shader.infoLog() /*<< std::endl*/;
	}

	return shader;
}
