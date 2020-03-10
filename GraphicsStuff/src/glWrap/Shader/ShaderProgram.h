#pragma once

#include <Common.h>

#include <Math/MathLib.h>
#include <glWrap/ID/Id.h>


namespace gl
{
	class Shader;

	using math::Float;
	using math::Int32;
	using math::Vec2;
	using math::Vec3;
	using math::Vec4;
	using math::Mat2;
	using math::Mat3;
	using math::Mat4;


	class ShaderProgram : public Id
	{
	private:
		static thread_local String INFO_LOG;


	public:
		struct ComputeInfo
		{
			GLuint m_numGroupsX = static_cast<GLuint>(0);
			GLuint m_numGroupsY = static_cast<GLuint>(0);
			GLuint m_numGroupsZ = static_cast<GLuint>(0);
		};


	public:
		ShaderProgram();

		ShaderProgram(ShaderProgram&& shaderProgram);


		~ShaderProgram();


		ShaderProgram& operator = (ShaderProgram&& shaderProgram);


	public:
		void createProgram();


		void attachShader(const Shader& shader) const;

		void detachShader(const Shader& shader) const;


		void link() const;

		void use() const;

		void unbind() const;


		void dispatchCompute() const;


		void setUniformMat4(GLint location, const Mat4& mat) const;

		void setUniformMat3(GLint location, const Mat3& mat) const;


		void setUniformVec4(GLint location, const Vec4& vec) const;

		void setUniformVec3(GLint location, const Vec3& vec) const;


		void setUniform1i(GLint location, Int32 value) const;

		void setUniform1f(GLint location, Float value) const;


		GLint getUniformLocation(const String& name) const;

		GLint getAttributeLocation(const String& name) const;


		void deleteProgram();


	public:
		bool linked() const;


		const String& infoLog() const;


		ComputeInfo& computeInfo();

		const ComputeInfo& computeInfo() const;


	private:
		void resetProgram();


	private:
		ComputeInfo m_computeInfo;
	};


	//aliases
	using ShaderProgramShared = std::shared_ptr<ShaderProgram>;
	using ShaderProgramWeak   = std::weak_ptr<ShaderProgram>;
}
