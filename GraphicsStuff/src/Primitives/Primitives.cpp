#include "Primitives.h"

namespace primitive
{
	using math::operator "" _FL;

	//utils
	Vec3 triangleNormal(const Triangle& triangle)
	{
		auto& [p0, p1, p2] = triangle.points;

		return glm::normalize(glm::cross(p1 - p0, p2 - p0));
	}

	Prism prismFromMiddleTriangleHeight(const Triangle& triangle, Float height)
	{
		Prism prism;

		auto& [p0, p1, p2] = triangle.points;

		Vec3 normal = triangleNormal(triangle);
		Vec3 half = normal * height / 2.0f;

		Vec3 p00 = p0 - half;
		Vec3 p01 = p1 - half;
		Vec3 p02 = p2 - half;
		Vec3 p10 = p0 + half;

		Vec3 normals[5];
		normals[Prism::LowerPlane] = +normal;
		normals[Prism::UpperPlane] = -normal;
		normals[Prism::SidePlane0] = glm::normalize(glm::cross(normal, p01 - p00));
		normals[Prism::SidePlane1] = glm::normalize(glm::cross(normal, p02 - p01));
		normals[Prism::SidePlane2] = glm::normalize(glm::cross(normal, p00 - p02));

		prism.planes[Prism::LowerPlane] = Vec4(normals[Prism::LowerPlane], -glm::dot(normals[Prism::LowerPlane], p00));
		prism.planes[Prism::UpperPlane] = Vec4(normals[Prism::UpperPlane], -glm::dot(normals[Prism::UpperPlane], p10));
		prism.planes[Prism::SidePlane0] = Vec4(normals[Prism::SidePlane0], -glm::dot(normals[Prism::SidePlane0], p00));
		prism.planes[Prism::SidePlane1] = Vec4(normals[Prism::SidePlane1], -glm::dot(normals[Prism::SidePlane1], p01));
		prism.planes[Prism::SidePlane2] = Vec4(normals[Prism::SidePlane2], -glm::dot(normals[Prism::SidePlane2], p02));

		return prism;
	}

	// doesn't check if points are same
	Cylinder cylinderFromPoints(const Vec3& point0, const Vec3& point1, Float radius)
	{
		Vec3  heightVec = point1 - point0;
		Float height = glm::length(heightVec);

		heightVec /= height;

		int i = 0;
		while (i < 3 && std::abs(heightVec[i]) < math::EPS)
		{
			i++;
		}
		if (i >= 3)
		{
			return Cylinder{};
		}

		int j = (i + 1) % 3;

		Vec3 radiusVec{};
		radiusVec[i] = +heightVec[j];
		radiusVec[j] = -heightVec[i];
		radiusVec /= glm::length(radiusVec);

		Cylinder cylinder{};
		cylinder.height = height;
		cylinder.heightVec = heightVec;
		cylinder.origin = point0;
		cylinder.radius = radius;
		cylinder.radiusVecs[0] = radiusVec;
		cylinder.radiusVecs[1] = glm::normalize(glm::cross(radiusVec, heightVec));
		return cylinder;
	}

	Sphere sphereFromCenterRadius(const Vec3& center, Float radius)
	{
		return Sphere{center, radius};
	}

	RoundedTriangle roundedTriangleFromTriangleRadius(const Triangle& triangle, Float radius, Float height)
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
		Float dh = glm::dot(delta, cylinder.heightVec);
		Float dr0 = glm::dot(delta, cylinder.radiusVecs[0]);
		Float dr1 = glm::dot(delta, cylinder.radiusVecs[1]);
		Float dr2 = dr0 * dr0 + dr1 * dr1;
		Float rr = (cylinder.radius * cylinder.radius);

		return 0.0_FL <= dh && dh <= cylinder.height && dr2 <= rr;
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
			auto& x0 = plane[0];
			auto& y0 = plane[1];
			auto& z0 = plane[2];
			auto& w0 = plane[3];

			float check = x * x0 + y * y0 + z * z0 + w0;
			if (check < 0.0_FL)
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

}
