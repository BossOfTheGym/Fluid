#include "QuadBuilder.h"

#include <Common.h>


//constructor
QuadBuilder::QuadBuilder()
{
	Vec2 vertices[6] = {
		  Vec2{+1.0f, +1.0f}
		, Vec2{-1.0f, +1.0f}
		, Vec2{-1.0f, -1.0f}

		, Vec2{-1.0f, -1.0f}
		, Vec2{+1.0f, -1.0f}
		, Vec2{+1.0f, +1.0f}
	};

	m_vertexPositions.bufferData(sizeof(vertices), vertices, BufferUsage::StaticDraw);
}


VertexArray QuadBuilder::buildShape()
{
	VertexArray quadShape(
		{
			  DrawMode::Triangles
			, 0 // first
		    , 6 // elements count(vertices count)
		}
	);

	quadShape.bind();
	quadShape.setAttribPointerInBuffer(
		  m_vertexPosition
		, {0, 2, DataType::Float, GLBool::True, 0, 0}
	);
	quadShape.unbind();

	return std::move(quadShape);
}