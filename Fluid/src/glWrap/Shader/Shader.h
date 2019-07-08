#pragma once

#include <Common.h>

#include <glWrap/OpenGL.h>
#include <glWrap/ID/Id.h>

class Shader : public Id
{
private:
    static thread_local String INFO_LOG;


public:
    enum class Type : GLenum
    {
          Vertex         = GL_VERTEX_SHADER
        , TessControl    = GL_TESS_CONTROL_SHADER
        , TessEvaluation = GL_TESS_EVALUATION_SHADER
        , Geometry       = GL_GEOMETRY_SHADER
        , Fragment       = GL_FRAGMENT_SHADER
		, Compute        = GL_COMPUTE_SHADER
        , None           = static_cast<GLenum>(0)
    };
	   

public:
	Shader();

    Shader(Type type, const String& location);

    Shader(Shader&& shader);


    ~Shader();


    Shader& operator = (Shader&& shader);


public:
    void deleteShader();



    Type type() const;

    bool compiled() const;


    const String& infoLog() const;


private:
    void resetShader();


    bool loadFromLocation(Type type, const String& location);

    bool loadFromStream(Type type, IStream& inputStream);

    bool loadFromString(Type type, const String& source);


private:    
    Type m_type;
};


//aliases
using ShaderShared = std::shared_ptr<Shader>;
using ShaderWeak   = std::weak_ptr<Shader>;