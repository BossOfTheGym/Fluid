#include "Buffer.h"


//statics
void Buffer::bindBuffer(Target target, Buffer& buffer)
{
	glBindBuffer(static_cast<GLenum>(target), buffer.id());
}

void Buffer::unbind(Target target)
{
	glBindBuffer(static_cast<GLenum>(target), EMPTY);
}


//constructors & destructor
Buffer::Buffer()
	: m_id(EMPTY)
	, m_size(0)
	, m_usage(Usage::None)
{}

Buffer::Buffer(GLsizeiptr pSize, const GLvoid* data, Usage pUsage)
	: Buffer()
{
	if (pUsage != Usage::None)
	{
		genBuffer();
		bufferData(pTarget, pSize, data, pUsage);
	}
}

Buffer::Buffer(Buffer&& buffer)
{
	//TODO
}

Buffer::~Buffer()
{
	deleteBuffer();
}


//core
void Buffer::genBuffer()
{
	glGenBuffers(1, &m_id);
}

void Buffer::bufferData(GLsizeiptr pSize, const GLvoid* data, Usage pUsage)
{
	glNamedBufferData(m_id, size, data, static_cast<GLenum>(pUsage));
}

void Buffer::deleteBuffer()
{
	glDeleteBuffers(1, &m_id);

	resetBuffer();
}


GLint Buffer::getBufferParameteriv()
{
	//TODO
	return {};
}

GLint64 Buffer::getBufferParameter64v()
{
	//TODO
	return {};
}


//private
void Buffer::resetBuffer()
{
	m_id     = EMPTY;
	m_usage  = Usage::None;
	m_size = 0;
}



//get
GLuint Buffer::id() const
{
	return m_id;
}

GLsizei Buffer::size() const
{
	return m_size;
}

GLenum Buffer::usage() const
{
	return m_usage;
}

GLbitfield Buffer::flags() const
{
	return m_flags;
}