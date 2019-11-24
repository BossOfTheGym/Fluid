#include "QuadBuilder.h"

#include <Common.h>
#include <Render/Attributes.h>

//constructor
QuadBuilder::QuadBuilder() 
	: m_vertexPositions()
	, m_textureCoords()
{
	Vec3 vertices[6] = {
		  Vec3{+1.0f, +1.0f, 0.0f}
		, Vec3{-1.0f, +1.0f, 0.0f}
		, Vec3{-1.0f, -1.0f, 0.0f}

		, Vec3{-1.0f, -1.0f, 0.0f}
		, Vec3{+1.0f, -1.0f, 0.0f}
		, Vec3{+1.0f, +1.0f, 0.0f}
	};

	Vec2 textureCoords[6] = {
		  Vec2(+1.0f, +1.0f)
		, Vec2(+0.0f, +1.0f)
		, Vec2(+0.0f, +0.0f)
		, Vec2(+0.0f, +0.0f)
		, Vec2(+1.0f, +0.0f)
		, Vec2(+1.0f, +1.0f)
	};

	m_vertexPositions.createBuffer();
	m_vertexPositions.namedBufferData(sizeof(vertices), vertices, BufferUsage::StaticDraw);

	m_textureCoords.createBuffer();
	m_textureCoords.namedBufferData(sizeof(textureCoords), textureCoords, BufferUsage::StaticDraw);
}


VertexArray QuadBuilder::buildShape()
{
	VertexArray quadShape;

	quadShape.info() = VertexArray::DrawInfo
	{
		DrawMode::Triangles
		, 0 // first
		, 6 // elements count(vertices count)
	};

	quadShape.genArray();
	quadShape.bind();
	quadShape.setAttribPointerInBuffer(
		m_vertexPositions
		, {
			  static_cast<GLuint>(VertexAttributes::Position)
			, AttributeSize::Three
		    , DataType::Float
		    , GLBool::True
		    , 0
		    , 0
		}
	);
	quadShape.setAttribPointerInBuffer(
		m_textureCoords
		, {
			  static_cast<GLuint>(VertexAttributes::Texture)
			, AttributeSize::Two
		    , DataType::Float
		    , GLBool::True
		    , 0
		    , 0
		}
	);
	quadShape.unbind();

	return quadShape;
}