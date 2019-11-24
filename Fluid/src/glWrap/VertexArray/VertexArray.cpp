#include "VertexArray.h"


//constructors & destructor
VertexArray::VertexArray()
	: Id()
	, m_info{}
{}

VertexArray::VertexArray(VertexArray&& vertexBuffer) noexcept : Id()
{
	*this = std::move(vertexBuffer);
}


VertexArray::~VertexArray()
{
    deleteArrayBuffer();
}


//operators
VertexArray& VertexArray::operator = (VertexArray&& vertexBuffer) noexcept
{
	static_cast<Id&>(*this) = static_cast<Id&&>(vertexBuffer);

	std::swap(m_info, vertexBuffer.m_info);

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
void VertexArray::genArray()
{
	GLuint arrayID;

	//creates only name, doesn't create inner state
	glGenVertexArrays(1, &arrayID);

	static_cast<Id&>(*this) = arrayID;
}

void VertexArray::createArray()
{
	GLuint arrayID;

	//creates name and inner object's state
	glCreateVertexArrays(1, &arrayID);

	static_cast<Id&>(*this) = arrayID;
}


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
		, static_cast<GLint>(info.size)
		, static_cast<GLenum>(info.type)
		, static_cast<GLboolean>(info.normalized)
		, info.stride
		, info.pointer
	);
}

void VertexArray::disableAttribArray(GLuint index)
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
void VertexArray::draw() const
{
	//TODO : rework this(there're plenty of draw* functions)
	if (m_info.indicesType == IndicesType::None)
	{
		drawArrays();
	}
	else
	{
		drawElements();
	}
}

void VertexArray::setAttribPointer(const PointerInfo& info)
{
	enableAttribArray(info.index);
	vertexAttribPointer(info);
}

void VertexArray::setAttribPointerInBuffer(const Buffer& buffer, const PointerInfo& info)
{
	buffer.bind(BufferTarget::ArrayBuffer);

	setAttribPointer(info);

	buffer.unbind(BufferTarget::ArrayBuffer);
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