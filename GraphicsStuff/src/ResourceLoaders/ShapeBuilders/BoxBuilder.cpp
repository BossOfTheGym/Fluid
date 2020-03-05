#include "BoxBuilder.h"

#include <cstdint>
#include <Render/Attributes.h>
#include <Math/MathLib.h>
#include <glWrap/Buffer/Buffer.h>
#include <glWrap/VertexArray/VertexArray.h>

namespace res
{
	using math::Vec3;
	using math::operator "" _FL;

	BoxBuilder::BoxBuilder()
		: m_vertexPositions()
		, m_indicesBuffer()
	{
		Vec3 vertices[8] =
		{
			  Vec3(-1.0_FL, -1.0_FL, -1.0_FL)
			, Vec3(-1.0_FL, -1.0_FL, +1.0_FL)
			, Vec3(-1.0_FL, +1.0_FL, -1.0_FL)
			, Vec3(-1.0_FL, +1.0_FL, +1.0_FL)
			, Vec3(+1.0_FL, -1.0_FL, -1.0_FL)
			, Vec3(+1.0_FL, -1.0_FL, +1.0_FL)
			, Vec3(+1.0_FL, +1.0_FL, -1.0_FL)
			, Vec3(+1.0_FL, +1.0_FL, +1.0_FL)
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
		m_vertexPositions.namedBufferData(sizeof(vertices), vertices, gl::BufferUsage::StaticDraw);

		m_indicesBuffer.createBuffer();
		m_indicesBuffer.namedBufferData(sizeof(indices), indices, gl::BufferUsage::StaticDraw);
	}


	gl::VertexArray BoxBuilder::buildShape()
	{
		gl::VertexArray result;

		result.info() = gl::VertexArray::DrawInfo
		{
			  gl::DrawMode::Triangles
			, static_cast<GLint>(0)
			, static_cast<GLsizei>(36)
			, gl::IndicesType::UnsignedInt
			, nullptr
		};

		result.genArray();
		result.bind();
		result.setAttribPointerInBuffer(
			m_vertexPositions
			, gl::VertexArray::PointerInfo
			{
				  static_cast<GLuint>(VertexAttributes::Position)
				, gl::AttributeSize::Three
				, gl::DataType::Float
				, gl::GLBool::True
				, static_cast<GLsizei>(0)
				, nullptr
			}
		);
		result.setElementsBuffer(m_indicesBuffer);
		result.unbind();

		return result;
	}
}
