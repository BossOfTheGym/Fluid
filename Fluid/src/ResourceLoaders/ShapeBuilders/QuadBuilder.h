#ifndef QUAD_BUILDER_H
#define QUAD_BUILDER_H


#include <glWrap/VertexArray/VertexArray.h>


class QuadBuilder
{
public:
	QuadBuilder();

	QuadBuilder(const QuadBuilder&) = delete;
	QuadBuilder(QuadBuilder&&)      = delete;

	~QuadBuilder() = default;

	QuadBuilder& operator = (const QuadBuilder&) = delete;
	QuadBuilder& operator = (QuadBuilder&&)      = delete;


public:
	VertexArray buildShape();


private:
	Buffer m_vertexPositions;
	Buffer m_textureCoords;
};

#endif