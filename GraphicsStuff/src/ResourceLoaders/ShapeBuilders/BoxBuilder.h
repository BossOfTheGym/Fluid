#pragma once

#include <glWrap/Buffer/Buffer.h>

namespace gl
{
	class VertexArray;
}

namespace res
{
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
		gl::VertexArray buildShape();


	private:
		gl::Buffer m_vertexPositions;
		gl::Buffer m_indicesBuffer;
	};
}
