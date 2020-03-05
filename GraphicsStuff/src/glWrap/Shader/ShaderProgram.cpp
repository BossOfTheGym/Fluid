#include "ShaderProgram.h"

#include <glWrap/Shader/Shader.h>

namespace gl
{
	//statics
	thread_local String ShaderProgram::INFO_LOG;


	//constructors & destructor
	ShaderProgram::ShaderProgram()
		: Id()
		, m_computeInfo{}
	{}

	ShaderProgram::ShaderProgram(ShaderProgram&& shaderProgram) noexcept : Id()
	{
		*this = std::move(shaderProgram);
	}


	ShaderProgram::~ShaderProgram()
	{
		deleteProgram();
	}


	//operators
	ShaderProgram& ShaderProgram::operator = (ShaderProgram&& shaderProgram) noexcept
	{
		static_cast<Id&>(*this) = static_cast<Id&&>(shaderProgram);

		std::swap(m_computeInfo, shaderProgram.m_computeInfo);

		return *this;
	}


	//core functions
	void ShaderProgram::createProgram()
	{
		static_cast<Id&>(*this) = glCreateProgram();
	}

	void ShaderProgram::attachShader(const Shader& shader) const
	{
		glAttachShader(id(), shader.id());
	}

	void ShaderProgram::detachShader(const Shader& shader) const
	{
		glDetachShader(id(), shader.id());
	}


	void ShaderProgram::link() const
	{
		glLinkProgram(id());
	}

	void ShaderProgram::use() const
	{
		glUseProgram(id());
	}

	void ShaderProgram::unbind() const
	{
		glUseProgram(Id::Empty);
	}


	//invoke compute shader(depends on openGL state, current program being in use)
	void ShaderProgram::dispatchCompute() const
	{
		glDispatchCompute(
			m_computeInfo.m_numGroupsX
			, m_computeInfo.m_numGroupsY
			, m_computeInfo.m_numGroupsZ
		);
	}


	//uniforms(depends on openGL state, current program being in use)
	void ShaderProgram::setUniformMat4(GLint location, const Mat4& mat) const
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
	}

	void ShaderProgram::setUniformMat3(GLint location, const Mat3& mat) const
	{
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
	}


	void ShaderProgram::setUniformVec4(GLint location, const Vec4& vec) const
	{
		glUniform4fv(location, 1, glm::value_ptr(vec));
	}

	void ShaderProgram::setUniformVec3(GLint location, const Vec3& vec) const
	{
		glUniform3fv(location, 1, glm::value_ptr(vec));
	}


	void ShaderProgram::setUniform1f(GLint location, Float value) const
	{
		glUniform1f(location, value);
	}


	//locations
	GLint ShaderProgram::getUniformLocation(const String& name) const
	{
		return glGetUniformLocation(id(), name.c_str());
	}

	GLint ShaderProgram::getAttributeLocation(const String& name) const
	{
		return glGetAttribLocation(id(), name.c_str());
	}


	//delete & reset
	void ShaderProgram::deleteProgram()
	{
		glDeleteProgram(id());

		resetProgram();
	}

	void ShaderProgram::resetProgram()
	{
		resetId();
	}


	//checks
	bool ShaderProgram::linked() const
	{
		GLint result;

		glGetProgramiv(id(), GL_LINK_STATUS, &result);

		return result == GL_TRUE;
	}


	//get
	ShaderProgram::ComputeInfo& ShaderProgram::computeInfo()
	{
		return m_computeInfo;
	}

	const ShaderProgram::ComputeInfo& ShaderProgram::computeInfo() const
	{
		return m_computeInfo;
	}


	//info log
	const String& ShaderProgram::infoLog() const
	{
		String& infoLog = ShaderProgram::INFO_LOG;

		GLint length;
		GLint returned;

		glGetProgramiv(id(), GL_INFO_LOG_LENGTH, &length);
		infoLog.resize(length);

		glGetProgramInfoLog(id(), length, &returned, infoLog.data());
		infoLog.resize(returned);

		return infoLog;
	}
}
