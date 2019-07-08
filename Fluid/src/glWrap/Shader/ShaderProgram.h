#pragma once

#include <Common.h>

#include <glWrap/Consts.h>
#include <glWrap/Shader/Shader.h>


class ShaderProgram
{
private:
    static thread_local String INFO_LOG;


public:
	static void unbind();


public:
	ShaderProgram();

    ShaderProgram(const String& name);

    ShaderProgram(const ShaderProgram& shaderProgram) = delete;

    ShaderProgram(ShaderProgram&& shaderProgram);


    ~ShaderProgram();


    ShaderProgram& operator = (const ShaderProgram& shaderProgram) = delete;

    ShaderProgram& operator = (ShaderProgram&& shaderProgram);



    void attachShader(const Shader& shader) const;

    void detachShader(const Shader& shader) const;


    void link() const;

    void use() const;


	void dispatchCompute(GLuint numGroupsX, GLuint numGroupsY, GLuint numGroupsZ) const;



    void setUniformMat4(GLint location, const Mat4& mat) const;

    void setUniformMat3(GLint location, const Mat3& mat) const;


    void setUniformVec4(GLint location, const Vec4& vec) const;

    void setUniformVec3(GLint location, const Vec3& vec) const;


    void setUniform1f(GLint location, double value) const;


    GLint getUniformLocation(const String& name) const;

    GLint getAttributeLocation(const String& name) const;



    void deleteProgram();


    bool valid() const;

    bool linked() const;



    GLuint id() const;
    
    const String& infoLog() const;


    virtual const String& toString() const;

private:
    void resetProgram();


private:
    GLuint mId;

    String mName;
};


//aliases
using ShaderProgramShared = std::shared_ptr<ShaderProgram>;
using ShaderProgramWeak   = std::weak_ptr<ShaderProgram>;