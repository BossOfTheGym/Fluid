#ifndef SHADER_PROGRAM_BUILDER_H
#define SHADER_PROGRAM_BUILDER_H

#include <glWrap/Shader/ShaderProgram.h>
#include <Utility/Apply.h>
#include <Utility/Holder.h>

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
	ShaderProgram buildProgram(Shaders&& ... shaders)
	{
		ShaderProgram program;

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

		return program;
	}
};

#endif