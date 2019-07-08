

#include "Shader.h"


//statics
thread_local String Shader::INFO_LOG;


//constructors
Shader::Shader()
	: Id()
	, m_type(Type::None)
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
	static_cast<Id&>(*this) = static_cast<Id&&>(shader);

	std::swap(m_type, shader.m_type);

	shader.resetShader();

    return *this;
}


//load functions
bool Shader::loadFromLocation(Type type, const String& location)
{
    IFStream file(location);

    if (!file.is_open())
    {
        return false;
    }

    return loadFromStream(type, file);
}

bool Shader::loadFromStream(Type type, IStream& inputStream)
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

bool Shader::loadFromString(Type type, const String& source)
{
    static_cast<Id&>(*this) = glCreateShader(static_cast<GLenum>(type));
    m_type = type;

    if (!valid())
    {
        resetShader();

        return false;
    }

    const GLchar* src = source.c_str();
    GLint size = source.size();

    glShaderSource(id(), 1, &src, &size);
    glCompileShader(id());

    if (!compiled())
    {
        return false;
    }

    return true;
}


//delete & reset
void Shader::deleteShader()
{
    glDeleteShader(id());

    resetShader();
}

void Shader::resetShader()
{
	resetId();

    m_type = Type::None;
}


//checks
bool Shader::compiled() const
{
    if(!valid())
    {
        return false;
    }

    GLint result;
    glGetShaderiv(id(), GL_COMPILE_STATUS, &result);

    return result == GL_TRUE;
}


const String& Shader::infoLog() const
{
    String& infoLog = Shader::INFO_LOG;

    GLint length;
    GLint returned;

    glGetShaderiv(id(), GL_INFO_LOG_LENGTH, &length);
    infoLog.resize(length);

    glGetShaderInfoLog(id(), length, &returned, infoLog.data());
    infoLog.resize(returned);

    return infoLog;
}


//get & set
Shader::Type Shader::type() const
{
    return m_type;
}
