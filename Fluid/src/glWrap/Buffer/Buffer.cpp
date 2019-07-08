#include "Buffer.h"



//constructors & destructor
namespace
{
	Id genBuffer()
	{
		GLuint bufferId;
		glGenBuffers(1, &bufferId);

		return bufferId;
	}
}


Buffer::Buffer()
	: Id()
	, m_size(0)
	, m_usage(Usage::None)
{}

Buffer::Buffer(GLsizeiptr size, const GLvoid* data, Usage usage)
	: Buffer()
{
	static_cast<Id&>(*this) = genBuffer();

	bufferData(size, data, usage);	
}

Buffer::Buffer(Buffer&& buffer)
{
	*this = std::move(buffer);
}

Buffer::~Buffer()
{
	deleteBuffer();
}


//operators
Buffer& Buffer::operator = (Buffer&& buffer)
{
	static_cast<Id&>(*this) = static_cast<Id&&>(buffer);

	std::swap(m_size , buffer.m_size);
	std::swap(m_usage, buffer.m_usage);

	buffer.resetBuffer();

	return *this;
}


//core
void Buffer::bind(Target target) const
{
	glBindBuffer(static_cast<GLenum>(target), id());
}

void Buffer::unbind(Target target) const
{
	glBindBuffer(static_cast<GLenum>(target), Id::Empty);
}

void Buffer::bufferData(GLsizeiptr size, const GLvoid* data, Usage usage)
{
	m_usage = usage;
	m_size  = size;

	glNamedBufferData(id(), size, data, static_cast<GLenum>(usage));
}

void Buffer::bufferSubData(GLintptr offset, GLsizei size, const GLvoid *data)
{
	glNamedBufferSubData(id(), offset, size, data);
}

void Buffer::deleteBuffer()
{
	glDeleteBuffers(1, &m_id);

	resetBuffer();
}

void Buffer::bindBufferBase()
{
	//TODO
}

void Buffer::bindBufferRange()
{
	//TODO
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
	resetId();

	m_usage  = Usage::None;
	m_size = 0;
}



//get
GLsizei Buffer::size() const
{
	return m_size;
}

Buffer::Usage Buffer::usage() const
{
	return m_usage;
}