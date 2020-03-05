#pragma once

#include <Math/MathLib.h>
#include <Mesh/Mesh.h>

namespace gl
{
	class VertexArray;
}

namespace misc
{
	class ArrowHeadMeshBuilder
	{
	public:
		using UInt32 = math::UInt32;
		using Float  = math::Float;

		ArrowHeadMeshBuilder(
			  UInt32 split    = 100
			, Float lengthFront = 2.71f
			, Float lengthBack  = 1.04f
			, Float widthMiddle = 2.08f
			, Float widthBack   = 2.5f
			, Float heightBack  = 0.5f
		);

		ArrowHeadMeshBuilder(const ArrowHeadMeshBuilder&) = delete;
		ArrowHeadMeshBuilder(ArrowHeadMeshBuilder&&)      = delete;

		~ArrowHeadMeshBuilder() = default;

		ArrowHeadMeshBuilder& operator = (const ArrowHeadMeshBuilder&) = delete;
		ArrowHeadMeshBuilder& operator = (ArrowHeadMeshBuilder&&)      = delete;

	public:
		const mesh::IndicesMesh& indicesMesh() const;

		const mesh::Mesh& mesh() const;


	private:
		mesh::IndicesMesh m_indicesMesh;
		mesh::Mesh m_mesh;

		UInt32 m_split;
		Float m_lengthFront;
		Float m_lengthBack;
		Float m_widthMiddle;
		Float m_widthBack;
		Float m_heightBack;
	};



	class ArrowHeadArrayBuilder
	{
	public:
		ArrowHeadArrayBuilder(const mesh::IndicesMesh& mesh);
		ArrowHeadArrayBuilder(mesh::IndicesMesh&& mesh);

		ArrowHeadArrayBuilder(const ArrowHeadArrayBuilder&) = delete;
		ArrowHeadArrayBuilder(ArrowHeadArrayBuilder&&)      = delete;

		~ArrowHeadArrayBuilder() = default;

		ArrowHeadArrayBuilder& operator = (const ArrowHeadArrayBuilder&) = delete;
		ArrowHeadArrayBuilder& operator = (ArrowHeadArrayBuilder&&)      = delete;


	public:
		gl::VertexArray buildShape();

	private:
		mesh::IndicesMesh m_mesh;
	};
}
