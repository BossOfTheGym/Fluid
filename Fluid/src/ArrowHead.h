#ifndef ARROW_HEAD_H
#define ARROW_HEAD_H


#include <glWrap/Vertexarray/VertexArray.h>

#include <cstdint>
#include "Mesh.h"


class ArrowHeadMeshBuilder
{
public:
	ArrowHeadMeshBuilder(
		  uint32_t split    = 100
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
	const IndicesMesh& indicesMesh() const;

	const Mesh& mesh() const;


private:
	IndicesMesh m_indicesMesh;
	Mesh m_mesh;

	uint32_t m_split   ;
	Float m_lengthFront;
	Float m_lengthBack ;
	Float m_widthMiddle;
	Float m_widthBack  ;
	Float m_heightBack ;
};


class ArrowHeadArrayBuilder
{
public:
	ArrowHeadArrayBuilder(const IndicesMesh& mesh);
	ArrowHeadArrayBuilder(IndicesMesh&& mesh);

	ArrowHeadArrayBuilder(const ArrowHeadArrayBuilder&) = delete;
	ArrowHeadArrayBuilder(ArrowHeadArrayBuilder&&)      = delete;

	~ArrowHeadArrayBuilder() = default;

	ArrowHeadArrayBuilder& operator = (const ArrowHeadArrayBuilder&) = delete;
	ArrowHeadArrayBuilder& operator = (ArrowHeadArrayBuilder&&)      = delete;


public:
	VertexArray buildShape();

private:
	IndicesMesh m_mesh;
};


#endif