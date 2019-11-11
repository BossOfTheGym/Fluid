#include "primitives.h"


//utils
//triangle points' order is ccw
Vec3 triangleNormal(const Triangle& triangle)
{
	auto& [p0, p1, p2] = triangle.points;

	return glm::normalize(glm::cross(p1 - p0, p2 - p0));
}

Prism prismFromMiddleTriangleHeight(const Triangle& triangle, float height)
{
	Prism prism;

	auto& [p0, p1, p2] = triangle.points;

	Vec3 normal = triangleNormal(triangle);
	Vec3 half   = normal * height / 2.0f;

	Vec3 p00 = p0 - half;
	Vec3 p01 = p1 - half;
	Vec3 p02 = p2 - half;
	Vec3 p10 = p0 + half;

	Vec3 normals[5];
	normals[Prism::LOWER_PLANE] = +normal;
	normals[Prism::UPPER_PLANE] = -normal;
	normals[Prism::SIDE_PLANE0] = glm::normalize(glm::cross(normal, p01 - p00));
	normals[Prism::SIDE_PLANE1] = glm::normalize(glm::cross(normal, p02 - p01));
	normals[Prism::SIDE_PLANE2] = glm::normalize(glm::cross(normal, p00 - p02));

	prism.planes[Prism::LOWER_PLANE] = Vec4(normals[Prism::LOWER_PLANE], -glm::dot(normals[Prism::LOWER_PLANE], p00));
	prism.planes[Prism::UPPER_PLANE] = Vec4(normals[Prism::UPPER_PLANE], -glm::dot(normals[Prism::UPPER_PLANE], p10));
	prism.planes[Prism::SIDE_PLANE0] = Vec4(normals[Prism::SIDE_PLANE0], -glm::dot(normals[Prism::SIDE_PLANE0], p00));
	prism.planes[Prism::SIDE_PLANE1] = Vec4(normals[Prism::SIDE_PLANE1], -glm::dot(normals[Prism::SIDE_PLANE1], p01));
	prism.planes[Prism::SIDE_PLANE2] = Vec4(normals[Prism::SIDE_PLANE2], -glm::dot(normals[Prism::SIDE_PLANE2], p02));

	return prism;
}

Cylinder cylinderFromPoints(const Vec3& point0, const Vec3& point1, float radius)
{
	Vec3  heightVec = point1 - point0;
	float height  = glm::length(heightVec);

	heightVec /= height;

	int i = 0;
	while (i < 3 && std::abs(heightVec[i]) < EPS)
	{
		i++;
	}

	int j = 0;
	if (i == 0)
	{
		j = 1;
	}

	Vec3 radiusVec{};
	radiusVec[i] = +heightVec[j];
	radiusVec[j] = -heightVec[i];
	radiusVec /= glm::length(radiusVec);

	Cylinder cylinder{};

	cylinder.height    = height;
	cylinder.heightVec = heightVec;
	cylinder.origin    = point0;
	cylinder.radius    = radius;
	cylinder.radiusVecs[0] = radiusVec;
	cylinder.radiusVecs[1] = glm::normalize(glm::cross(radiusVec, heightVec));

	return cylinder;
}

Sphere sphereFromCenterRadius(const Vec3& center, float radius)
{
	return Sphere{center, radius};
}

RoundedTriangle roundedTriangleFromTriangleRadius(const Triangle& triangle, float radius, float height)
{
	auto& [p0, p1, p2] = triangle.points;

	RoundedTriangle result;

	result.vertices[0] = sphereFromCenterRadius(p0, radius);
	result.vertices[1] = sphereFromCenterRadius(p1, radius);
	result.vertices[2] = sphereFromCenterRadius(p2, radius);
	result.edges[0] = cylinderFromPoints(p0, p1, radius);
	result.edges[1] = cylinderFromPoints(p1, p2, radius);
	result.edges[2] = cylinderFromPoints(p2, p0, radius);
	result.triangle = prismFromMiddleTriangleHeight(triangle, height);

	return result;
}



//volume checks
bool pointInCylinder(const Vec3& point, const Cylinder& cylinder)
{
	Vec3 delta = point - cylinder.origin;
	float dh  = glm::dot(delta, cylinder.heightVec);
	float dr0 = glm::dot(delta, cylinder.radiusVecs[0]);
	float dr1 = glm::dot(delta, cylinder.radiusVecs[1]);
	float dr2 = dr0 * dr0 + dr1 * dr1;
	float rr  = (cylinder.radius * cylinder.radius);

	return 0 <= dh && dh <= cylinder.height && dr2 <= rr;
}

bool pointInSphere(const Vec3& point, const Sphere& sphere)
{
	Vec3 dr = point - sphere.center;

	return glm::dot(dr, dr) <= sphere.radius * sphere.radius;
}

bool pointInPrism(const Vec3& point, const Prism& prism)
{
	auto& x = point[0];
	auto& y = point[1];
	auto& z = point[2];
	for (auto& plane : prism.planes)
	{
		auto& x0 = plane[ 0];
		auto& y0 = plane[1];
		auto& z0 = plane[2];
		auto& w0 = plane[3];

		float check = x * x0 + y * y0 + z * z0 + w0;
		if (check <= 0.0f)
		{
			return false;
		}
	}

	return true;
}

bool pointInRoundedTriangle(const Vec3& point, const RoundedTriangle& roundedTriangle)
{
	auto& [vertices, edges, triangle] = roundedTriangle;

	return pointInSphere(point, vertices[0])
		|| pointInSphere(point, vertices[1])
		|| pointInSphere(point, vertices[2])
		|| pointInCylinder(point, edges[0])
		|| pointInCylinder(point, edges[1])
		|| pointInCylinder(point, edges[2])
		|| pointInPrism(point, triangle);
}