#include "ShaderLoader.h"

#include <glWrap/Shader/Shader.h>

namespace res
{
	gl::Shader SimpleShaderLoader::loadShader(gl::ShaderType type, const String& location)
	{
		gl::Shader shader;


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
			std::cout << "Failed to compile shader: " << location << std::endl;
			std::cout << shader.infoLog() << std::endl;
		}

		return shader;
	}
}
