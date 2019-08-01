#include "VertexArray.h"


//constructors & destructor
namespace
{
	Id genVertexArray()
	{
		GLuint vertexArrayId;
		glGenVertexArrays(1, &vertexArrayId);

		return vertexArrayId;
	}
}

VertexArray::VertexArray(const DrawInfo& info) 
	: Id(genVertexArray())
	, m_info(info)
{}

VertexArray::VertexArray(VertexArray&& vertexBuffer) : Id()
{
	*this = std::move(vertexBuffer);
}


VertexArray::~VertexArray()
{
    deleteArrayBuffer();
}


//operators
VertexArray& VertexArray::operator = (VertexArray&& vertexBuffer)
{
	static_cast<Id&>(*this) = static_cast<Id&&>(vertexBuffer);

	m_info = vertexBuffer.m_info;

	vertexBuffer.deleteArrayBuffer();

    return *this;
}


//delete & reset
void VertexArray::deleteArrayBuffer()
{
	GLuint vertexArrayId = id();
    glDeleteVertexArrays(1, &vertexArrayId);

    resetArrayBuffer();
}


void VertexArray::resetArrayBuffer()
{
	resetId();
}


//core functions
void VertexArray::bind() const
{
    glBindVertexArray(id());
}

void VertexArray::unbind() const
{
	glBindVertexArray(Id::Empty);

	glBindBuffer(static_cast<GLenum>(BufferTarget::ArrayBuffer)       , Id::Empty);
	glBindBuffer(static_cast<GLenum>(BufferTarget::ElementArrayBuffer), Id::Empty);
}

void VertexArray::enableAttribArray(GLuint index)
{
    glEnableVertexAttribArray(index);
}

void VertexArray::vertexAttribPointer(const PointerInfo& info)
{
	glVertexAttribPointer(
		  info.index
		, info.size
		, static_cast<GLenum>(info.type)
		, static_cast<GLboolean>(info.normalized)
		, info.stride
		, info.pointer
	);
}

void VertexArray::disableAttribPointer(GLuint index)
{
    glDisableVertexAttribArray(index);
}

void VertexArray::drawArrays() const
{
	glDrawArrays(
		static_cast<GLenum>(m_info.drawMode)
		, m_info.first
		, m_info.elementsCount
	);
}

void VertexArray::drawElements() const
{
	glDrawElements(
		static_cast<GLenum>(m_info.drawMode)
		, m_info.elementsCount
		, static_cast<GLenum>(m_info.indicesType)
		, m_info.indices
	);
}


//misc
void VertexArray::setAttribPointer(const PointerInfo& info)
{
	enableAttribArray(info.index);
	vertexAttribPointer(info);
}

void VertexArray::setAttribPointerInBuffer(const Buffer& buffer, const PointerInfo& info)
{
	buffer.bind(BufferTarget::ArrayBuffer);

	setAttribPointer(info);
}

void VertexArray::setElementsBuffer(const Buffer& buffer)
{
	buffer.bind(BufferTarget::ElementArrayBuffer);
}


//get & set
VertexArray::DrawInfo& VertexArray::info()
{
	return m_info;
}

const VertexArray::DrawInfo& VertexArray::info() const
{
	return m_info;
}