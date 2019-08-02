#ifndef SHADER_H
#define SHADER_H

#include <Common.h>

#include <glWrap/OpenGL.h>
#include <glWrap/ID/Id.h>

class Shader : public Id
{
private:
    static thread_local String INFO_LOG;

public:
	Shader();

	Shader(ShaderType type);

    Shader(ShaderType type, const String& source);

    Shader(Shader&& shader);


    ~Shader();


    Shader& operator = (Shader&& shader);


public:
	void createShader(ShaderType type);

	void shaderSource(const String& shader);

	void compileShader();

    void deleteShader();


public:
    ShaderType type() const;

    bool compiled() const;


    const String& infoLog() const;


private:
    void resetShader();


private:    
    ShaderType m_type;
};


//aliases
using ShaderShared = std::shared_ptr<Shader>;
using ShaderWeak   = std::weak_ptr<Shader>;

#endif