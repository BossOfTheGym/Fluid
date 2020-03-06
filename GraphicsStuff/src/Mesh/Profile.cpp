#include "Profile.h"


namespace mesh
{
	using math::Int32;
	using math::Vec3;
	using math::Vec4;
	using math::Mat3;
	using math::Mat4;

	using math::operator "" _FL;

	Profile readProfile(const std::string& filename)
	{
		std::ifstream input(filename);

		Int32 size;
		input >> size;

		Profile profile;
		profile.args.resize(size);
		profile.vals.resize(size);
		profile.count = size;
		for (auto& arg : profile.args)
		{
			input >> arg;
		}
		for (auto& val : profile.vals)
		{
			input >> val;
		}

		return profile;
	}

	Mesh constructMeshFromProfile(const Profile& profile, Float yLower, Float yUpper, Int32 angleSplit)
	{
		using Points = std::vector<Vec3>;

		auto getPoints = [&] (const auto& profile) -> Points
		{
			Points result;

			result.reserve(profile.count);
			for (int i = 0; i < profile.count; i++)
			{
				auto& val = profile.vals[i];
				auto& arg = profile.args[i];

				result.push_back(Vec3(val, arg, 0.0_FL));
			}

			return result;
		};

		auto getRotationMat = [&] (Int32 split) -> Mat3
		{
			Mat3 mat(1.0_FL);

			Float cosAngle = std::cos(math::PI2 / split);
			Float sinAngle = std::sin(math::PI2 / split);

			mat[0][0] = +cosAngle; mat[2][0] = -sinAngle;
			mat[0][2] = +sinAngle; mat[2][2] = +cosAngle;

			return mat;
		};

		auto rotatePoints = [&] (Points& points, const Mat3& mat) -> void
		{
			for (auto& point : points)
			{
				point = mat * point;
			}
		};


		Mesh result;
		Points back  = getPoints(profile);
		Points front = getPoints(profile);
		Mat3 rotation = getRotationMat(angleSplit);

		result.triangles.reserve(2ll * profile.count * angleSplit);
		for (Int32 k = 0; k < angleSplit; k++)
		{
			back = front;
			rotatePoints(front, rotation);

			//add lower triangle
			result.triangles.push_back(Triangle{back.front(), front.front(), Vec3(0.0_FL, yLower, 0.0_FL)});

			//add profile triangles
			for (Int32 i = 0; i < profile.count - 1; i++)
			{
				result.triangles.push_back(Triangle{back[i], front[i], front[i + 1]});
				result.triangles.push_back(Triangle{front[i + 1],  back[i + 1],  back[i]});
			}

			//add upper triangle
			result.triangles.push_back(Triangle{back.back(), front.back(), Vec3(0.0_FL, yUpper, 0.0_FL)});
		}

		return result;
	}
}