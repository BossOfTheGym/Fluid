#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <Common.h>


struct Triangle
{
	Vec3 points[3] = {};
};

struct Cylinder
{
	Vec3 heightVec     = {}; // normalized
	Vec3 radiusVecs[2] = {}; // normalized
	Vec3 origin        = {};
	float height = {};
	float radius = {};
};

struct Prism
{
	enum
	{
		  LOWER_PLANE = 0
		, UPPER_PLANE = 1
		, SIDE_PLANE0 = 2
		, SIDE_PLANE1 = 3
		, SIDE_PLANE2 = 4
	};

	Vec4 planes[5];
};

struct Sphere
{
	Vec3 center{};
	float radius{};
};

struct AABB
{
	Vec3 corners[2] = {};
};

struct RoundedTriangle
{
	Sphere   vertices[3] = {};
	Cylinder edges[3]    = {};
	Prism    triangle    = {};
};


//utils
//triangle points' order is ccw
Vec3 triangleNormal(const Triangle& triangle);

Prism prismFromMiddleTriangleHeight(const Triangle& triangle, float height);

Cylinder cylinderFromPoints(const Vec3& point0, const Vec3& point1, float radius);

Sphere sphereFromCenterRadius(const Vec3& center, float radius);

RoundedTriangle roundedTriangleFromTriangleRadius(const Triangle& triangle, float radius, float height);



//volume checks
bool pointInCylinder(const Vec3& point, const Cylinder& cylinder);

bool pointInSphere(const Vec3& point, const Sphere& sphere);

bool pointInPrism(const Vec3& point, const Prism& prism);

bool pointInRoundedTriangle(const Vec3& point, const RoundedTriangle& roundedTriangle);

#endif