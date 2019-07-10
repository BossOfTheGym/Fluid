#pragma once

#include <Common.h>

#include <glWrap/ID/Id.h>
#include <glWrap/Shader/Shader.h>


class ShaderProgram : public Id
{
private:
    static thread_local String INFO_LOG;


public:
	struct ComputeInfo
	{
		GLuint m_numGroupsX;
		GLuint m_numGroupsY;
		GLuint m_numGroupsZ;
	};


public:
	ShaderProgram(
		  const String& name = "Shader program"
		, const ComputeInfo& computeInfo = ComputeInfo{0, 0, 0}
	);

    ShaderProgram(ShaderProgram&& shaderProgram);


    ~ShaderProgram();


    ShaderProgram& operator = (ShaderProgram&& shaderProgram);


public:
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


    void setUniform1f(GLint location, double value) const;


    GLint getUniformLocation(const String& name) const;

    GLint getAttributeLocation(const String& name) const;


    void deleteProgram();


public:
    bool linked() const;


	const String& name() const;

    const String& infoLog() const;


	ComputeInfo& computeInfo();

	const ComputeInfo& computeInfo() const;


private:
    void resetProgram();


private:
    String      m_name;
	ComputeInfo m_computeInfo;
};


//aliases
using ShaderProgramShared = std::shared_ptr<ShaderProgram>;
using ShaderProgramWeak   = std::weak_ptr<ShaderProgram>;