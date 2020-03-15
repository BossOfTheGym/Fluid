#pragma once

#include <Math/MathLib.h>
#include <tuple>

namespace primitive
{
	using math::Float;
	using math::Vec3;
	using math::Vec4;
	using math::Mat3;

	// TODO : add structure binding support for every primitive
	// to make it's usage common
	// ex.:         auto [p0, p1, p2] = triangle 
	// (instead of) auto [p0, p1, p2] = triangle.points 
	// p0, p1, p2 - appropriate points of triangle

	struct Triangle
	{
		//triangle points' order is ccw
		Vec3 points[3] = {};
	};

	struct Cylinder
	{
		// round as a base and heightVec orthogonal to base
		Vec3 heightVec     = {}; // normalized
		Vec3 radiusVecs[2] = {}; // normalized
		Vec3 origin  = {};
		Float height = {};
		Float radius = {};
	};

	struct SECylinder
	{
		// SECylinder : shear elliptical cylinder
		// has ellipse as base and heightVec is not orthogonal to base
		Vec3 heightVec{};   // normalized
		Vec3 apseVec{};     // normalized
		Vec3 periapseVec{}; // normalized
		Vec3 origin{};

		Mat3 gramm{}; // gramm matrix of basis dot products, basis(apseVec, periapseVec, heightVec)
		Float det{};  // gramm matrix determinant

		Float height{};
		Float apse{};
		Float periapse{};
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

	struct AAEllipsoid
	{
		// AA - axis aligned
		Vec3 center{}; // center of an eleipse
		Vec3 abcSq{};   // vector (a^2, b^2, c^2)
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

	struct ShearRoundedTriangle
	{
		AAEllipsoid vertices[3] = {};
		SECylinder  edges[3] = {};
		Prism       triangle = {};
	};

	// utils
	Vec3 triangleNormal(const Triangle& triangle);

	std::tuple<Vec3, Float> triangleNormalLength(const Triangle& triangle);

	Vec3 triangleHeightVec(const Triangle& triangle);


	// construct
	Prism prismFromExtrudedNormalTriangle(const Triangle& triangle, Float height);

	Prism prismFromExtrudedTriangleVec(const Triangle& triangle, const Vec3& dir);

	// doesn't check if points are same
	Cylinder cylinderFromPoints(const Vec3& point0, const Vec3& point1, Float radius);

	// TODO : mb remove
	SECylinder SECylinderFromVecs(const Vec3& origin, const Vec3& height, const Vec3& apse, const Vec3& periapse);

	Sphere sphereFromCenterRadius(const Vec3& center, Float radius);

	// TODO : mb remove
	AAEllipsoid AAEllipseFromCenterExtents(const Vec3& center, const Vec3& extents);

	RoundedTriangle roundedTriangleFromRadius(const Triangle& triangle, Float vertexRadius, Float edgeRadius, Float height);

	// TODO : mb remove
	// shear : vector of coefficients of stretching along the xyz axes
	// rel   : point relative to we stretch
	ShearRoundedTriangle shearRoundedTriangleFromRadiusHeight(
		const Triangle& triangle, Float radius, Float height, const Vec3& shear, const Vec3& rel
	);

	//volume checks
	bool pointInCylinder(const Vec3& point, const Cylinder& cylinder);

	// TODO : mb remove
	bool pointInSECylinder(const Vec3& point, const SECylinder& cylinder);

	bool pointInSphere(const Vec3& point, const Sphere& sphere);

	// TOOD : mb remove
	bool pointinAAEllipse(const Vec3& point, const AAEllipsoid& ellipse);

	bool pointInPrism(const Vec3& point, const Prism& prism);

	bool pointInRoundedTriangle(const Vec3& point, const RoundedTriangle& roundedTriangle);
	
	bool pointInAABB(const Vec3& point, const AABB& aabb);

	// same name
	bool pointInVolume(const Vec3& point, const Cylinder& cylinder);


	// TODO : mb remove
	bool pointInVolume(const Vec3& point, const SECylinder& cylinder);

	bool pointInVolume(const Vec3& point, const Sphere& sphere);

	// TODO : mb remove
	bool pointInVolume(const Vec3& point, const AAEllipsoid& aaEllipse);

	bool pointInVolume(const Vec3& point, const Prism& prism);

	bool pointInVolume(const Vec3& point, const RoundedTriangle& volume);

	bool pointInVolume(const Vec3& point, const AABB& aabb);
}