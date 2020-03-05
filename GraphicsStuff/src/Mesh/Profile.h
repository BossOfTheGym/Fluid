#pragma once


#include <fstream>
#include <vector>
#include <string>
#include <algorithm>


#include "Mesh.h"


namespace mesh
{
	using math::Float;
	using math::Int32;

	struct Profile
	{
		//args belong to [-1, +1]
		//vals belong to [-1, +1]
		std::vector<Float> vals{};
		std::vector<Float> args{};

		Int32 count{};
	};


	Profile readProfile(const std::string& filename);

	Mesh constructMeshFromProfile(const Profile& profile, Float yLower, Float yUpper, Int32 angleSplit);
}
