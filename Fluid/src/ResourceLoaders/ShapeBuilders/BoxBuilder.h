#ifndef BOX_BUILDER_H
#define BOX_BUILDER_H


#include <glWrap/VertexArray/VertexArray.h>

class BoxBuilder
{
public:
	BoxBuilder();

	BoxBuilder(BoxBuilder&&)      = delete;
	BoxBuilder(const BoxBuilder&) = delete;

	~BoxBuilder() = default;

	BoxBuilder& operator = (BoxBuilder&&)      = delete;
	BoxBuilder& operator = (const BoxBuilder&) = delete;


public:
	VertexArray buildShape();


private:
	Buffer m_vertexPositions;
	Buffer m_indicesBuffer;
};

#endif