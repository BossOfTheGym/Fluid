#include "ShaderLoader.h"

Shader SimpleShaderLoader::loadShader(ShaderType type, const String& location)
{
	m_buffer.clear();

	IFStream inputFile(location);

	m_buffer.assign(
		  std::istreambuf_iterator<char>(inputFile)
		, std::istreambuf_iterator<char>()
	);

	return Shader(type, m_buffer);
}
