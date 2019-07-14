#pragma once

#include <glWrap/OpenGL.h>
#include <glWrap/ID/Id.h>

class Buffer : public Id
{
public:
public:
	Buffer();

	Buffer(GLsizeiptr size, const GLvoid* data, BufferUsage usage);

	Buffer(Buffer&& buffer);

	~Buffer();


	Buffer& operator = (Buffer&& buffer);


public:
	void bind(BufferTarget target) const;

	void unbind(BufferTarget target) const;

	void deleteBuffer();

	void bufferData(GLsizeiptr pSize, const GLvoid* data, BufferUsage pUsage);
	
	void bufferSubData(GLintptr offset, GLsizei size, const GLvoid *data);

	void bindBufferBase();

	void bindBufferRange();


	GLint getBufferParameteriv();
	
	GLint64 getBufferParameter64v();


	GLsizei size() const;

	BufferUsage usage() const;


private:
	void resetBuffer();


private:
	GLsizei m_size;
	BufferUsage   m_usage;
};