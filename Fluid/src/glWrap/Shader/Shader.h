#pragma once

#include <Common.h>

#include <glWrap/OpenGL.h>
#include <glWrap/ID/Id.h>

class Shader : public Id
{
private:
    static thread_local String INFO_LOG;

public:
	Shader(ShaderType type = ShaderType::None);

    Shader(ShaderType type, const String& location);

    Shader(Shader&& shader);


    ~Shader();


    Shader& operator = (Shader&& shader);


public:
    void deleteShader();



    ShaderType type() const;

    bool compiled() const;


    const String& infoLog() const;


private:
    void resetShader();


    bool loadFromLocation(ShaderType type, const String& location);

    bool loadFromStream(ShaderType type, IStream& inputStream);

    bool loadFromString(ShaderType type, const String& source);


private:    
    ShaderType m_type;
};


//aliases
using ShaderShared = std::shared_ptr<Shader>;
using ShaderWeak   = std::weak_ptr<Shader>;