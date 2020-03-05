#pragma once

#include <glWrap/Shader/ShaderProgram.h>

namespace res
{
	class ShaderProgramBuilder
	{
	public:
		ShaderProgramBuilder() = default;

		ShaderProgramBuilder(const ShaderProgramBuilder&) = delete;
		ShaderProgramBuilder(ShaderProgramBuilder&&)      = delete;

		~ShaderProgramBuilder() = default;

		ShaderProgramBuilder& operator = (const ShaderProgramBuilder&) = delete;
		ShaderProgramBuilder& operator = (ShaderProgramBuilder&&)      = delete;


	public:
		template<class ... Shaders>
		gl::ShaderProgram buildProgram(Shaders&& ... shaders)
		{
			gl::ShaderProgram program;

			program.createProgram();

			(program.attachShader(shaders), ...);

			program.link();
			if (program.linked())
			{
				std::cout << "Success. Program built." << std::endl;
			}
			else
			{
				std::cout << "Failed to build program. " << program.infoLog();
			}

			(program.detachShader(shaders), ...);

			return program;
		}
	};
}
