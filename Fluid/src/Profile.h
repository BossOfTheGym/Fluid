#ifndef PROFILE_H
#define PROFILE_H

#include "Mesh.h"

#include <fstream>
#include <vector>
#include <string>
#include <algorithm>


template<class Float>
struct Profile
{
	//args belong to [-1, +1]
	//vals belong to [-1, +1]

	std::vector<Float> vals{};
	std::vector<Float> args{};

	int count{};
};


template<class Float>
auto readProfile(const std::string& filename)
{
	std::ifstream input(filename);

	int size;
	input >> size;

	Profile<Float> profile;

	profile.args.resize(size);
	profile.vals.resize(size);
	profile.size = size;
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


template<class Float>
Mesh constructMeshFromProfile(const Profile<Float>& profile, Float yLower, Float yUpper, int angleSplit)
{
	using Points = std::vector<Vec3>;

	auto getPoints = [&] (const Profile<Float>& profile) -> Points
	{
		Points result;

		result.reserve(profile.count);
		for (int i = 0; i < profile.count; i++)
		{
			auto& val = profile.vals[i];
			auto& arg = profile.args[i];

			result.push_back(Vec3(val, arg, 0.0f));
		}

		return result;
	};

	auto getRotationMat = [&] (int split) -> Mat3
	{
		Mat3 mat(1.0f);

		Float cosAngle = std::cos(PI2 / split);
		Float sinAngle = std::sin(PI2 / split);

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
	for (int k = 0; k < angleSplit; k++)
	{
		back = front;
		rotatePoints(front, rotation);

		//add lower triangle
		result.triangles.push_back(Triangle{back.front(), front.front(), Vec3(0.0f, yLower, 0.0f)});

		//add profile triangles
		for (int i = 0; i < profile.count - 1; i++)
		{
			result.triangles.push_back(Triangle{ back[i    ], front[i    ], front[i + 1]});
			result.triangles.push_back(Triangle{front[i + 1],  back[i + 1],  back[i    ]});
		}

		//add upper triangle
		result.triangles.push_back(Triangle{back.back(), front.back(), Vec3(0.0f, yUpper, 0.0f)});
	}

	return result;
}


#endif