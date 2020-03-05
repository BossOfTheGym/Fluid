#include "QuadBuilder.h"

#include <Common.h>
#include <Math/MathLib.h>
#include <Render/Attributes.h>
#include <glWrap/Buffer/Buffer.h>
#include <glWrap/VertexArray/VertexArray.h>

namespace res
{
	using math::Vec2;
	using math::Vec3;
	using math::operator "" _FL;

	//constructor
	QuadBuilder::QuadBuilder()
		: m_vertexPositions()
		, m_textureCoords()
	{
		Vec3 vertices[6] ={
			  Vec3{+1.0_FL, +1.0_FL, 0.0_FL}
			, Vec3{-1.0_FL, +1.0_FL, 0.0_FL}
			, Vec3{-1.0_FL, -1.0_FL, 0.0_FL}

			, Vec3{-1.0_FL, -1.0_FL, 0.0_FL}
			, Vec3{+1.0_FL, -1.0_FL, 0.0_FL}
			, Vec3{+1.0_FL, +1.0_FL, 0.0_FL}
		};

		Vec2 textureCoords[6] ={
			  Vec2(+1.0_FL, +1.0_FL)
			, Vec2(+0.0_FL, +1.0_FL)
			, Vec2(+0.0_FL, +0.0_FL)
			, Vec2(+0.0_FL, +0.0_FL)
			, Vec2(+1.0_FL, +0.0_FL)
			, Vec2(+1.0_FL, +1.0_FL)
		};

		m_vertexPositions.createBuffer();
		m_vertexPositions.namedBufferData(sizeof(vertices), vertices, gl::BufferUsage::StaticDraw);

		m_textureCoords.createBuffer();
		m_textureCoords.namedBufferData(sizeof(textureCoords), textureCoords, gl::BufferUsage::StaticDraw);
	}


	gl::VertexArray QuadBuilder::buildShape()
	{
		gl::VertexArray quadShape;

		quadShape.info() = gl::VertexArray::DrawInfo
		{
			gl::DrawMode::Triangles
			, 0 // first
			, 6 // elements count(vertices count)
		};

		quadShape.genArray();
		quadShape.bind();
		quadShape.setAttribPointerInBuffer(
			m_vertexPositions
			, {
				  static_cast<GLuint>(VertexAttributes::Position)
				, gl::AttributeSize::Three
				, gl::DataType::Float
				, gl::GLBool::True
				, 0
				, 0
			}
		);
		quadShape.setAttribPointerInBuffer(
			m_textureCoords
			, {
				  static_cast<GLuint>(VertexAttributes::Texture)
				, gl::AttributeSize::Two
				, gl::DataType::Float
				, gl::GLBool::True
				, 0
				, 0
			}
		);
		quadShape.unbind();

		return quadShape;
	}
}
