#include "Primitives.h"

// TODO : make all comparisons EPS-correct
// TODO : rework little bit cylinder construction & pointInCylinder check
namespace primitive
{
	using math::operator "" _FL;

	//utils
	Vec3 triangleNormal(const Triangle& triangle)
	{
		return glm::normalize(triangleHeightVec(triangle));
	}

	std::tuple<Vec3, Float> triangleNormalLength(const Triangle& triangle)
	{
		auto heightVec = triangleHeightVec(triangle);
		auto length    = glm::length(heightVec);
		return {heightVec / length, length};
	}

	Vec3 triangleHeightVec(const Triangle& triangle)
	{
		auto& [p0, p1, p2] = triangle.points;

		return glm::cross(p1 - p0, p2 - p0);
	}


	Prism prismFromExtrudedNormalTriangle(const Triangle& triangle, Float height)
	{
		Prism prism;

		auto& [p0, p1, p2] = triangle.points;
		auto e01 = p1 - p0;
		auto e12 = p2 - p1;
		auto e02 = p2 - p0;
		Vec3 normal = glm::normalize(glm::cross(e01, e02));

		Vec3 normals[5];
		normals[Prism::LowerPlane] = +normal;
		normals[Prism::UpperPlane] = -normal;
		normals[Prism::SidePlane0] = glm::cross(normal, +e01);
		normals[Prism::SidePlane1] = glm::cross(normal, +e12);
		normals[Prism::SidePlane2] = glm::cross(normal, -e02);

		Vec3 h = height * normal;
		prism.planes[Prism::LowerPlane] = Vec4(normals[Prism::LowerPlane], -glm::dot(normals[Prism::LowerPlane], p0 - h));
		prism.planes[Prism::UpperPlane] = Vec4(normals[Prism::UpperPlane], -glm::dot(normals[Prism::UpperPlane], p0 + h));
		prism.planes[Prism::SidePlane0] = Vec4(normals[Prism::SidePlane0], -glm::dot(normals[Prism::SidePlane0], p0));
		prism.planes[Prism::SidePlane1] = Vec4(normals[Prism::SidePlane1], -glm::dot(normals[Prism::SidePlane1], p1));
		prism.planes[Prism::SidePlane2] = Vec4(normals[Prism::SidePlane2], -glm::dot(normals[Prism::SidePlane2], p2));

		return prism;
	}

	Prism prismFromExtrudedTriangleVec(const Triangle& triangle, const Vec3& dir)
	{
		Prism prism;

		auto& [p0, p1, p2] = triangle.points;
		auto e01 = p1 - p0;
		auto e12 = p2 - p1;
		auto e02 = p2 - p0;
		Vec3 height = glm::cross(e01, e02);

		Vec3 halfDir = dir / 2.0_FL;
		if (glm::dot(height, halfDir) < 0)
		{
			halfDir *= -1.0_FL;
		}
		Vec3 normals[5];
		normals[Prism::LowerPlane] = +height;
		normals[Prism::UpperPlane] = -height;
		normals[Prism::SidePlane0] = glm::cross(halfDir, +e01);
		normals[Prism::SidePlane1] = glm::cross(halfDir, +e12);
		normals[Prism::SidePlane2] = glm::cross(halfDir, -e02);

		prism.planes[Prism::LowerPlane] = Vec4(normals[Prism::LowerPlane], -glm::dot(normals[Prism::LowerPlane], p0 - halfDir));
		prism.planes[Prism::UpperPlane] = Vec4(normals[Prism::UpperPlane], -glm::dot(normals[Prism::UpperPlane], p0 + halfDir));
		prism.planes[Prism::SidePlane0] = Vec4(normals[Prism::SidePlane0], -glm::dot(normals[Prism::SidePlane0], p0));
		prism.planes[Prism::SidePlane1] = Vec4(normals[Prism::SidePlane1], -glm::dot(normals[Prism::SidePlane1], p1));
		prism.planes[Prism::SidePlane2] = Vec4(normals[Prism::SidePlane2], -glm::dot(normals[Prism::SidePlane2], p2));

		return prism;
	}

	Cylinder cylinderFromPoints(const Vec3& point0, const Vec3& point1, Float radius)
	{
		// doesn't check if points are same

		Vec3  heightVec = point1 - point0;

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

		Cylinder cylinder{};
		cylinder.origin = point0;
		cylinder.hh = glm::dot(heightVec, heightVec);
		cylinder.rr = radius * radius;
		cylinder.heightVec = heightVec;
		cylinder.radiusVecs[0] = radius * glm::normalize(radiusVec);
		cylinder.radiusVecs[1] = radius * glm::normalize(glm::cross(radiusVec, heightVec));
		return cylinder;
	}

	SECylinder SECylinderFromVecs(const Vec3& origin, const Vec3& height, const Vec3& apse, const Vec3& periapse)
	{
		SECylinder result;
		result.origin = origin;
		result.height      = glm::length(height);
		result.heightVec   = height / result.height;
		result.apse        = glm::length(apse);
		result.apseVec     = apse / result.apse;
		result.periapse    = glm::length(periapse);
		result.periapseVec = periapse / result.periapse;

		auto& u0 = result.apse;
		auto& u1 = result.periapse;
		auto& u2 = result.height;
		auto& g  = result.gramm; 
		auto u0u1 = glm::dot(u0, u1);
		auto u1u2 = glm::dot(u1, u2);
		auto u0u2 = glm::dot(u0, u2);
		g[0][0] = 1.0_FL; g[1][0] = u0u1;   g[2][0] = u0u2;
		g[0][1] = u0u1;   g[1][1] = 1.0_FL; g[2][1] = u1u2;
		g[0][2] = u0u2;   g[1][2] = u1u2;   g[2][2] = 1.0_FL;
		result.det = glm::determinant(g);
		return result;
	}

	Sphere sphereFromCenterRadius(const Vec3& center, Float radius)
	{
		return Sphere{center, radius};
	}

	AAEllipsoid AAEllipseFromCenterExtents(const Vec3& center, const Vec3& extents)
	{
		return AAEllipsoid{center, extents * extents};
	}

	RoundedTriangle roundedTriangleFromRadius(const Triangle& triangle,  Float vertexRadius, Float edgeRadius, Float height)
	{
		auto& [p0, p1, p2] = triangle.points;

		RoundedTriangle result;

		result.vertices[0] = sphereFromCenterRadius(p0, vertexRadius);
		result.vertices[1] = sphereFromCenterRadius(p1, vertexRadius);
		result.vertices[2] = sphereFromCenterRadius(p2, vertexRadius);
		result.edges[0] = cylinderFromPoints(p0, p1, edgeRadius);
		result.edges[1] = cylinderFromPoints(p1, p2, edgeRadius);
		result.edges[2] = cylinderFromPoints(p2, p0, edgeRadius);
		result.triangle = prismFromExtrudedNormalTriangle(triangle, height);

		return result;
	}

	ShearRoundedTriangle shearRoundedTriangleFromRadiusHeight(
		const Triangle& triangle, Float radius, Float height, const Vec3& shear, const Vec3& rel
	)
	{
		//Vec3 p0 = triangle.points[0] - rel;
		//Vec3 p1 = triangle.points[1] - rel;
		//Vec3 p2 = triangle.points[2] - rel;

		//auto e01 = p1 - p0;
		//auto e12 = p2 - p1;
		//auto e20 = p0 - p2;

		//auto getAP = [&] (const auto& dir)
		//{
		//	// get apse periapse
		//	if (dir.z != 0.0_FL)
		//	{
		//		auto v1 = Vec3(dir.y, -dir.x, 0.0_FL);
		//		auto v2 = glm::cross();
		//	}
		//	return std::make_tuple();
		//};

		//{
		//	
		//}
		return {};
	}


	//volume checks
	bool pointInCylinder(const Vec3& point, const Cylinder& cylinder)
	{
		auto& r0 = cylinder.radiusVecs[0];
		auto& r1 = cylinder.radiusVecs[1];
		auto& h  = cylinder.heightVec;
		auto& rr = cylinder.rr;
		auto& hh = cylinder.hh;

		auto d = point - cylinder.origin;

		auto r0c = glm::dot(d, r0) / rr;
		auto r1c = glm::dot(d, r1) / rr;
		auto hc  = glm::dot(d, h)  / hh;

		return 0.0_FL <= hc && hc <= 1.0_FL && r0c * r0c + r1c * r1c <= 1.0_FL;
	}

	bool pointInSECylinder(const Vec3& point, const SECylinder& cylinder)
	{
		auto& g = cylinder.gramm;
		auto& a = cylinder.apse;
		auto& p = cylinder.periapse;
		auto& det = cylinder.det;

		auto d = point - cylinder.origin;
		auto v = Vec3(glm::dot(d, g[0]), glm::dot(d, g[1]), glm::dot(d, g[2]));
		auto ac = glm::dot(v   , glm::cross(g[1], g[2])) / det;
		auto pc = glm::dot(g[0], glm::cross(v   , g[2])) / det;
		auto hc = glm::dot(g[0], glm::cross(g[1], v   )) / det;

		return 0.0_FL <= hc && hc <= cylinder.height
			&& ac * ac / (a * a) + pc * pc / (p * p) <= 1.0_FL;
	}

	bool pointInSphere(const Vec3& point, const Sphere& sphere)
	{
		Vec3 dr = point - sphere.center;

		return glm::dot(dr, dr) <= sphere.radius * sphere.radius;
	}

	bool pointinAAEllipse(const Vec3& point, const AAEllipsoid& ellipse)
	{
		auto& [c, abcSq] = ellipse;
		auto p = point - c;

		auto check = p * p / abcSq;

		return check[0] + check[1] + check[2] == 1.0_FL;
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

			Float check = x * x0 + y * y0 + z * z0 + w0;
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

	bool pointInAABB(const Vec3& point, const AABB& aabb)
	{
		auto& [fc, sc] = aabb.corners;
		return fc[0] <= point[0] && point[0] <= sc[0]
			&& fc[1] <= point[1] && point[1] <= sc[1]
			&& fc[2] <= point[2] && point[2] <= sc[2];
	}

	// same name
	bool pointInVolume(const Vec3& point, const Cylinder& cylinder)
	{
		return pointInCylinder(point, cylinder);
	}

	bool pointInVolume(const Vec3& point, const SECylinder& cylinder)
	{
		return pointInSECylinder(point, cylinder);
	}

	bool pointInVolume(const Vec3& point, const Sphere& sphere)
	{
		return pointInSphere(point, sphere);
	}

	bool pointInVolume(const Vec3& point, const AAEllipsoid& aaEllipse)
	{
		return pointinAAEllipse(point, aaEllipse);
	}

	bool pointInVolume(const Vec3& point, const Prism& prism)
	{
		return pointInPrism(point, prism);
	}

	bool pointInVolume(const Vec3& point, const RoundedTriangle& volume)
	{
		return pointInRoundedTriangle(point, volume);
	}

	bool pointInVolume(const Vec3& point, const AABB& aabb)
	{
		return pointInAABB(point, aabb);
	}
}
