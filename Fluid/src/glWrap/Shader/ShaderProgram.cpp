#include "ShaderProgram.h"


//statics
thread_local String ShaderProgram::INFO_LOG;

void ShaderProgram::unbind()
{
	glUseProgram(EMPTY);
}

//constructors & destructor
ShaderProgram::ShaderProgram()
	: mId(EMPTY)
	, mName("")
{}

ShaderProgram::ShaderProgram(const String& name) 
    : mId(glCreateProgram())
    , mName(name)
{}

ShaderProgram::ShaderProgram(ShaderProgram&& shaderProgram)
{
	*this = std::move(shaderProgram);
}


ShaderProgram::~ShaderProgram()
{
    deleteProgram();
}


//operators
ShaderProgram& ShaderProgram::operator = (ShaderProgram&& shaderProgram)
{
	if (this != &shaderProgram)
	{
		std::swap(mId, shaderProgram.mId);
	}

    return *this;
}


//core functions
void ShaderProgram::attachShader(const Shader& shader) const
{
    glAttachShader(mId, shader.id());
}

void ShaderProgram::detachShader(const Shader& shader) const
{
    glDetachShader(mId, shader.id());
}


void ShaderProgram::link() const
{
    glLinkProgram(mId);
}

void ShaderProgram::use() const
{
    glUseProgram(mId);
}


void ShaderProgram::dispatchCompute(GLuint numGroupsX, GLuint numGroupsY, GLuint numGroupsZ) const
{
	glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
}


//uniforms
void ShaderProgram::setUniformMat4(GLint location, const Mat4& mat) const
{
    glUniformMatrix4dv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::setUniformMat3(GLint location, const Mat3& mat) const
{
    glUniformMatrix3dv(location, 1, GL_FALSE, glm::value_ptr(mat));
}


void ShaderProgram::setUniformVec4(GLint location, const Vec4& vec) const
{
    glUniform4dv(location, 1, glm::value_ptr(vec));
}

void ShaderProgram::setUniformVec3(GLint location, const Vec3& vec) const
{
    glUniform3dv(location, 1, glm::value_ptr(vec));
}


void ShaderProgram::setUniform1f(GLint location, double value) const
{
    glUniform1d(location, value);
}


//locations
GLint ShaderProgram::getUniformLocation(const String& name) const
{
    return glGetUniformLocation(mId, name.c_str());
}

GLint ShaderProgram::getAttributeLocation(const String& name) const
{
    return glGetAttribLocation(mId, name.c_str());
}


//delete & reset
void ShaderProgram::deleteProgram()
{
    glDeleteProgram(mId);

    resetProgram();
}

void ShaderProgram::resetProgram()
{
    mId = EMPTY;
    mName = "";
}



//fake virtual
const String& ShaderProgram::toString() const
{
    return mName;
}

//checks
bool ShaderProgram::valid() const
{
    return mId != EMPTY;
}

bool ShaderProgram::linked() const
{
    GLint result;

    glGetProgramiv(mId, GL_LINK_STATUS, &result);

    return result == GL_TRUE;
}


//get
GLuint ShaderProgram::id() const
{
    return mId;
}


//info log
const String& ShaderProgram::infoLog() const
{
    String& infoLog = ShaderProgram::INFO_LOG;

    GLint length;
    GLint returned;

    glGetProgramiv(mId, GL_INFO_LOG_LENGTH, &length);
    infoLog.resize(length);

    glGetProgramInfoLog(mId, length, &returned, infoLog.data());
    infoLog.resize(returned);

    return infoLog;
}