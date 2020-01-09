#ifndef ARROW_HEAD_H
#define ARROW_HEAD_H


#include <glWrap/Vertexarray/VertexArray.h>

#include <Render/Attributes.h>

#include <cstdint>
#include <Common.h>

class ArrowHeadBuilder
{
public:
	ArrowHeadBuilder(
		  uint32_t split    = 50
	    , Float lengthFront = 2.0f
		, Float lengthBack  = 3.0f
		, Float widthMiddle = 2.0f
		, Float widthBack   = 3.0f
		, Float heightBack  = 1.0f
		, Float sigma       = 1.0f
	);

	ArrowHeadBuilder(const ArrowHeadBuilder&) = delete;
	ArrowHeadBuilder(ArrowHeadBuilder&&)      = delete;

	~ArrowHeadBuilder() = default;

	ArrowHeadBuilder& operator = (const ArrowHeadBuilder&) = delete;
	ArrowHeadBuilder& operator = (ArrowHeadBuilder&&)      = delete;


public:
	VertexArray buildShape();


private:
	uint32_t m_split    = 10;
	Float m_lengthFront = 2.0f;
	Float m_lengthBack  = 3.0f;
	Float m_widthMiddle = 2.0f;
	Float m_widthBack   = 3.0f;
	Float m_heightBack  = 1.0f;
	Float m_sigma       = 0.2f;
};


#endif