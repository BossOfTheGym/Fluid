

#include "Shader.h"


//statics
thread_local String Shader::INFO_LOG;


//constructors
Shader::Shader()
	: mId(EMPTY)
	, mType(Invalid)
{}

Shader::Shader(Type type, const String& location) : Shader()
{
    loadFromLocation(type, location);
}

Shader::Shader(Shader&& shader)
{
	*this = std::move(shader);
}


//destructor
Shader::~Shader()
{
    deleteShader();
}


//operators
Shader& Shader::operator = (Shader&& shader)
{
	if (this != &shader)
	{
		std::swap(mId, shader.mId);
		std::swap(mType, shader.mType);
	}

    return *this;
}


//load functions
int Shader::loadFromLocation(Type type, const String& location)
{
    IFStream file(location);

    if (!file.is_open())
    {
        return false;
    }

    return loadFromStream(type, file);
}

int Shader::loadFromStream(Type type, IStream& inputStream)
{
    String source;
    String line;

    while (inputStream.good())
    {
        std::getline(inputStream, line);

        source += line;
        source += '\n';
    }

    if (!inputStream.eof())
    {
        return false;
    }

    return loadFromString(type, source);
}

int Shader::loadFromString(Type type, const String& source)
{
    if (type == Type::Invalid)
    {
        return false;
    }

    mId   = glCreateShader(type);
    mType = type;

    if (mId == EMPTY)
    {
        resetShader();

        return false;
    }

    const GLchar* src = source.c_str();
    GLint size = source.size();

    glShaderSource(mId, 1, &src, &size);
    glCompileShader(mId);

    if (!compiled())
    {
        return false;
    }

    return true;
}


//delete & reset
void Shader::deleteShader()
{
    glDeleteShader(mId);

    resetShader();
}

void Shader::resetShader()
{
    mId   = EMPTY;
    mType = Type::Invalid;
}


//checks
bool Shader::valid() const
{
    return mId != EMPTY;
}

bool Shader::compiled() const
{
    if(!valid())
    {
        return false;
    }

    GLint result;
    glGetShaderiv(mId, GL_COMPILE_STATUS, &result);

    return result == GL_TRUE;
}


const String& Shader::infoLog() const
{
    String& infoLog = Shader::INFO_LOG;

    GLint length;
    GLint returned;

    glGetShaderiv(mId, GL_INFO_LOG_LENGTH, &length);
    infoLog.resize(length);

    glGetShaderInfoLog(mId, length, &returned, infoLog.data());
    infoLog.resize(returned);

    return infoLog;
}


//get & set
GLuint Shader::id() const
{
    return mId;
}

Shader::Type Shader::type() const
{
    return mType;
}
