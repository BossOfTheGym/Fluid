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

VertexArray::VertexArray(const VertexArray::DrawInfo& info) 
	: Id(genVertexArray())
	, m_info(info)
{}

VertexArray::VertexArray(VertexArray&& vertexBuffer)
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
	//TODO
	static_cast<Id&>(*this) = static_cast<Id&&>(vertexBuffer);

	m_info = vertexBuffer.m_info;

	vertexBuffer.resetArrayBuffer();

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
}

void VertexArray::setAttribPointer(GLuint index, GLint size, GLenum element, GLsizei stride, const void* offset)
{
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, element, GL_FALSE, stride, offset);
}

void VertexArray::enableAttribArray(GLuint index)
{
    glEnableVertexAttribArray(index);
}

void VertexArray::disableAttribPointer(GLuint index)
{
    glDisableVertexAttribArray(index);
}

void VertexArray::drawArrays() const
{
	glDrawArrays(
		static_cast<GLenum>(m_info.m_drawMode)
		, m_info.m_first
		, m_info.m_elementsCount
	);
}

void VertexArray::drawElements() const
{
	glDrawElements(
		static_cast<GLenum>(m_info.m_drawMode)
		, m_info.m_elementsCount
		, static_cast<GLenum>(m_info.m_indicesType)
		, m_info.m_indices
	);
}


//misc



//get & set
VertexArray::DrawInfo& VertexArray::info()
{
	return m_info;
}

const VertexArray::DrawInfo& VertexArray::info() const
{
	return m_info;
}