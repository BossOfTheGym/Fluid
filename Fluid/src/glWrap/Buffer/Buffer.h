#pragma once

#include <glWrap/Consts.h>


class Buffer
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
		, None                    = static_cast<GLenum>(EMPTY)
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
	static void bindBuffer(Target target, Buffer& buffer);

	static void unbind();


public:
	Buffer();

	Buffer(GLsizeiptr pSize, const GLvoid* data, Usage pUsage);

	Buffer(const Buffer&) = delete;
	Buffer(Buffer&& buffer);

	~Buffer();

	Buffer& operator = (const Buffer&) = delete;
	Buffer& operator = (Buffer&& buffer);


public:
	void deleteBuffer();

	void genBuffer();

	void bufferData(GLsizeiptr pSize, const GLvoid* data, Usage pUsage);
	
	void bindBufferBase();

	void bindBufferRange();


	GLint getBufferParameteriv();
	
	GLint64 getBufferParameter64v();


	bool valid() const;


	GLuint id() const;

	GLsizei size() const;

	GLenum usage() const;


private:
	void resetBuffer();


private:
	GLuint     m_id;
	GLsizei    m_size;
	GLenum     m_usage;
};