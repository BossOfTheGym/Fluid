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

	//only AtomicCounterBuffer, TransformFeedbackBuffer, UniformBuffer, ShaderStorageBuffer allowed
	void bindBufferBase(BufferTarget target, GLuint index);

	void bindBufferRange(BufferTarget target, GLuint index, GLintptr offset, GLsizeiptr size);

	void unbindBufferBase(BufferTarget target, GLuint index);


	GLint getBufferParameteriv();
	
	GLint64 getBufferParameter64v();


	GLsizei size() const;

	BufferUsage usage() const;


private:
	void resetBuffer();


private:
	GLsizei       m_size;
	BufferUsage   m_usage;
};