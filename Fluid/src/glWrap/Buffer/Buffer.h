#pragma once

#include <glWrap/OpenGL.h>
#include <glWrap/ID/Id.h>

class Buffer : public Id
{
public:
public:
	Buffer();

	Buffer(GLsizeiptr size, const GLvoid* data, Usage usage);

	Buffer(Buffer&& buffer);

	~Buffer();


	Buffer& operator = (Buffer&& buffer);


public:
	void bind(Target target) const;

	void unbind(Target target) const;

	void deleteBuffer();

	void bufferData(GLsizeiptr pSize, const GLvoid* data, Usage pUsage);
	
	void bufferSubData(GLintptr offset, GLsizei size, const GLvoid *data);

	void bindBufferBase();

	void bindBufferRange();


	GLint getBufferParameteriv();
	
	GLint64 getBufferParameter64v();


	GLsizei size() const;

	Usage usage() const;


private:
	void resetBuffer();


private:
	GLsizei m_size;
	Usage   m_usage;
};