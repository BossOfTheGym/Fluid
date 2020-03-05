#pragma once

#include <glWrap/Buffer/Buffer.h>

namespace gl
{
	class VertexArray;
}

namespace res
{
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
		gl::VertexArray buildShape();


	private:
		gl::Buffer m_vertexPositions;
		gl::Buffer m_textureCoords;
	};
}
