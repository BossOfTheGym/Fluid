#pragma once

#include <Math/MathLib.h>

namespace primitive
{
	using math::Float;
	using math::Vec3;
	using math::Vec4;

	//triangle points' order is ccw
	struct Triangle
	{
		Vec3 points[3] = {};
	};

	struct Cylinder
	{
		Vec3 heightVec     = {}; // normalized
		Vec3 radiusVecs[2] = {}; // normalized
		Vec3 origin  = {};
		Float height = {};
		Float radius = {};
	};

	struct Prism
	{
		enum
		{
			  LowerPlane = 0
			, UpperPlane = 1
			, SidePlane0 = 2
			, SidePlane1 = 3
			, SidePlane2 = 4
		};

		Vec4 planes[5];
	};

	struct Sphere
	{
		Vec3 center{};
		Float radius{};
	};

	struct AABB
	{
		Vec3 corners[2] = {};
	};

	struct RoundedTriangle
	{
		Sphere   vertices[3] = {};
		Cylinder edges[3] = {};
		Prism    triangle = {};
	};


	//utils
	Vec3 triangleNormal(const Triangle& triangle);

	Prism prismFromMiddleTriangleHeight(const Triangle& triangle, Float height);

	// doesn't check if points are same
	Cylinder cylinderFromPoints(const Vec3& point0, const Vec3& point1, Float radius);

	Sphere sphereFromCenterRadius(const Vec3& center, Float radius);

	RoundedTriangle roundedTriangleFromTriangleRadius(const Triangle& triangle, Float radius, Float height);



	//volume checks
	bool pointInCylinder(const Vec3& point, const Cylinder& cylinder);

	bool pointInSphere(const Vec3& point, const Sphere& sphere);

	bool pointInPrism(const Vec3& point, const Prism& prism);

	bool pointInRoundedTriangle(const Vec3& point, const RoundedTriangle& roundedTriangle);
	
	bool pointInAABB(const Vec3& point, const AABB& aabb);
}