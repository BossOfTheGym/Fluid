#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <fstream>

#include <cmath>
#include <utility>
#include <thread>
#include <memory>
#include <limits>

#include <filesystem>
#include <functional>

#include <string>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//math
namespace detail
{
	template<class FloatType>
	constexpr FloatType eps_value();

	template<>
	constexpr float eps_value<float>()
	{
		return 1e-6f;
	}

	template<>
	constexpr double eps_value<double>()
	{
		return 1e-15;
	}
}


using Int   = int;
using UInt  = unsigned int;
using Float = float;

const Float EPS = detail::eps_value<Float>();
const Float SQ2 = std::sqrt(2.0f);
const Float SQ3 = std::sqrt(3.0f);

const Float PI   = static_cast<Float>(std::acos(-1.0));
const Float PI2  = 2 * PI;
const Float PI_2 = PI / 2;
const Float PI_4 = PI / 4;
const Float PI_5 = PI / 5;
const Float PI_6 = PI / 6;
const Float PI_8 = PI / 8;


using Vec4 = glm::tvec4<Float>;
using Mat4 = glm::tmat4x4<Float>;

using Vec3 = glm::tvec3<Float>;
using Mat3 = glm::tmat3x3<Float>;

using Vec2 = glm::tvec2<Float>;
using Mat2 = glm::tmat2x2<Float>;


using Vec3i = glm::tvec3<Int>;


//STL aliases 
using String = std::string;

using IStream = std::istream;
using OStream = std::ostream;

using IFStream = std::ifstream;
using OFStream = std::ofstream;

#endif