#include "BoxBuilder.h"

#include <cstdint>
#include <Render/Attributes.h>

BoxBuilder::BoxBuilder()
	: m_vertexPositions()
	, m_indicesBuffer()
{
	Vec3 vertices[8] = 
	{
		  Vec3(-1.0f, -1.0f, -1.0f)
		, Vec3(-1.0f, -1.0f, +1.0f)
		, Vec3(-1.0f, +1.0f, -1.0f)
		, Vec3(-1.0f, +1.0f, +1.0f)
		, Vec3(+1.0f, -1.0f, -1.0f)
		, Vec3(+1.0f, -1.0f, +1.0f)
		, Vec3(+1.0f, +1.0f, -1.0f)
		, Vec3(+1.0f, +1.0f, +1.0f)
	};

	GLuint indices[36] = 
	{
		  3, 7, 2
		, 6, 2, 7

		, 6, 7, 4
		, 5, 4, 7

		, 7, 3, 5
		, 1, 5, 3

		, 3, 2, 1
		, 0, 1, 2

		, 2, 6, 0
		, 4, 0, 6

		, 4, 5, 0
		, 1, 0, 5
	};


	m_vertexPositions.createBuffer();
	m_vertexPositions.namedBufferData(sizeof(vertices), vertices, BufferUsage::StaticDraw);

	m_indicesBuffer.createBuffer();
	m_indicesBuffer.namedBufferData(sizeof(indices), indices, BufferUsage::StaticDraw);
}


VertexArray BoxBuilder::buildShape()
{
	VertexArray result;

	result.info() = VertexArray::DrawInfo
	{
		  DrawMode::Triangles
		, static_cast<GLint>(0)
		, static_cast<GLsizei>(36)
		, IndicesType::UnsignedInt
		, nullptr
	};

	result.genArray();
	result.bind();
	result.setAttribPointerInBuffer(
		m_vertexPositions
		, VertexArray::PointerInfo
		{
			  static_cast<GLuint>(VertexAttributes::Position)
			, AttributeSize::Three
			, DataType::Float
			, GLBool::True
		    , static_cast<GLsizei>(0)
		    , nullptr
		}
	);
	result.setElementsBuffer(m_indicesBuffer);
	result.unbind();

	return result;
}