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
	, m_usage(BufferUsage::None)
{}

Buffer::Buffer(GLsizeiptr size, const GLvoid* data, BufferUsage usage)
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
void Buffer::bind(BufferTarget target) const
{
	glBindBuffer(static_cast<GLenum>(target), id());
}

void Buffer::unbind(BufferTarget target) const
{
	glBindBuffer(static_cast<GLenum>(target), Id::Empty);
}

void Buffer::bufferData(GLsizeiptr size, const GLvoid* data, BufferUsage usage)
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
	GLuint bufferId = id();
	glDeleteBuffers(1, &bufferId);

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

	m_usage  = BufferUsage::None;
	m_size = 0;
}



//get
GLsizei Buffer::size() const
{
	return m_size;
}

BufferUsage Buffer::usage() const
{
	return m_usage;
}