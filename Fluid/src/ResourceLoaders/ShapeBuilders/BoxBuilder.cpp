#include "BoxBuilder.h"

#include <cstdint>
#include <Render/Attributes.h>

BoxBuilder::BoxBuilder()
	: m_vertexPositions()
	, m_indicesBuffer()
{
	Vec3 vertices[8] = {
		  Vec3(-1.0f, -1.0f, -1.0f)
		, Vec3(-1.0f, -1.0f, +1.0f)
		, Vec3(-1.0f, +1.0f, -1.0f)
		, Vec3(-1.0f, +1.0f, +1.0f)
		, Vec3(+1.0f, -1.0f, -1.0f)
		, Vec3(+1.0f, -1.0f, +1.0f)
		, Vec3(+1.0f, +1.0f, -1.0f)
		, Vec3(+1.0f, +1.0f, +1.0f)
	};

	GLuint indices[24] = {
		  6u, 4u, 7u, 5u
		, 7u, 5u, 3u, 1u
		, 3u, 1u, 2u, 0u
		, 2u, 0u, 6u, 4u
		, 6u, 7u, 2u, 3u
		, 5u, 4u, 1u, 0u
	};


	m_vertexPositions.genBuffer();
	m_vertexPositions.bufferData(sizeof(vertices), vertices, BufferUsage::StaticDraw);

	m_indicesBuffer.genBuffer();
	m_indicesBuffer.bufferData(sizeof(indices), indices, BufferUsage::StaticDraw);
}


VertexArray BoxBuilder::buildShape()
{
	VertexArray result;

	result.info() = VertexArray::DrawInfo{
		DrawMode::TriangleStrip
		, static_cast<GLint>(0)
		, static_cast<GLsizei>(24)
		, IndicesType::UnsignedInt
		, nullptr
	};

	result.genArray();
	result.bind();
	result.setAttribPointerInBuffer(
		m_vertexPositions
		, VertexArray::PointerInfo{
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