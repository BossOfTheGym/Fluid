#pragma once

#include <Common.h>

#include <glWrap/OpenGL.h>
#include <glWrap/ID/Id.h>

class Shader
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
        , Invalid        = 0
    };
	   

public:
	Shader();

    Shader(Type type, const String& location);

    Shader(const Shader& shader) = delete;

    Shader(Shader&& shader);


    ~Shader();


    Shader& operator = (const Shader& shader) = delete;

    Shader& operator = (Shader&& shader);


    void deleteShader();



    GLuint id() const;

    Type type() const;


    bool valid() const;

    bool compiled() const;


    const String& infoLog() const;


private:
    void resetShader();


    int loadFromLocation(Type type, const String& location);

    int loadFromStream(Type type, IStream& inputStream);

    int loadFromString(Type type, const String& source);


private:    
    GLuint mId;
    Type mType;
};


//aliases
using ShaderShared = std::shared_ptr<Shader>;
using ShaderWeak   = std::weak_ptr<Shader>;