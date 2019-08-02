#include "Buffer.h"



//constructors & destructor
Buffer::Buffer()
	: Id()
	, m_size(0)
	, m_usage(BufferUsage::None)
{}

Buffer::Buffer(GLsizeiptr size, const GLvoid* data, BufferUsage usage)
	: Buffer()
{
	genBuffer();

	bufferData(size, data, usage);	
}

Buffer::Buffer(Buffer&& buffer) : Id()
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

	buffer.deleteBuffer();

	return *this;
}


//core
void Buffer::genBuffer()
{
	GLuint bufferId;

	glGenBuffers(1, &bufferId);

	static_cast<Id&>(*this) = bufferId;
}

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

void Buffer::bindBufferBase(BufferTarget target, GLuint index)
{
	glBindBufferBase(static_cast<GLenum>(target), index, id());
}

void Buffer::bindBufferRange(BufferTarget target, GLuint index, GLintptr offset, GLsizeiptr size)
{
	glBindBufferRange(static_cast<GLenum>(target), index, id(), offset, size);
}

void Buffer::unbindBufferBase(BufferTarget target, GLuint index)
{
	glBindBufferBase(static_cast<GLenum>(target), index, Id::Empty);
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
GLsizeiptr Buffer::size() const
{
	return m_size;
}

BufferUsage Buffer::usage() const
{
	return m_usage;
}