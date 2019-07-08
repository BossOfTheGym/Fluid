#pragma once

#include <glWrap/OpenGL.h>
#include <glWrap/ID/Id.h>

class Buffer : public Id
{
public:
	enum class Target : GLenum
	{
		  ArrayBuffer             = GL_ARRAY_BUFFER
		, AtomicCounterBuffer     = GL_ATOMIC_COUNTER_BUFFER
		, CopyReadBuffer          = GL_COPY_READ_BUFFER
		, CopyWriteBuffer         = GL_COPY_WRITE_BUFFER
		, DispatchIndirectBuffer  = GL_DISPATCH_INDIRECT_BUFFER
		, DrawIndirectBuffer      = GL_DRAW_INDIRECT_BUFFER
		, ElementArrayBuffer      = GL_ELEMENT_ARRAY_BUFFER
		, PixelUnpackBuffer       = GL_PIXEL_UNPACK_BUFFER
		, PixelPackBuffer         = GL_PIXEL_PACK_BUFFER
		, QueryBuffer             = GL_QUERY_BUFFER
		, ShaderStorageBuffer     = GL_SHADER_STORAGE_BUFFER
		, TextureBuffer           = GL_TEXTURE_BUFFER
		, TransformFeedbackBuffer = GL_TRANSFORM_FEEDBACK_BUFFER
		, UniformBuffer           = GL_UNIFORM_BUFFER
		, None                    = static_cast<GLenum>(Id::Empty)
	};

	enum class Usage : GLenum
	{
		  StreamDraw  = GL_STREAM_DRAW
		, StreamRead  = GL_STREAM_READ
		, StreamCopy  = GL_STREAM_COPY
		, StaticDraw  = GL_STATIC_DRAW
		, StaticRead  = GL_STATIC_READ
		, StaticCopy  = GL_STATIC_COPY
		, DynamicDraw = GL_DYNAMIC_DRAW
		, DynamicRead = GL_DYNAMIC_READ
		, DynamicCopy = GL_DYNAMIC_COPY
		, None        = static_cast<GLenum>(0)
	};


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