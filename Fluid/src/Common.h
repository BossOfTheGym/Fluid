#pragma once

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


const double PI   = static_cast<double>(acos(-1.0));
const double PI_2 = static_cast<double>(2 * PI);
 

using UInt = unsigned int;
using Float = float;


using Vec4 = glm::tvec4<Float>;
using Mat4 = glm::tmat4x4<Float>;

using Vec3 = glm::tvec3<Float>;
using Mat3 = glm::tmat3x3<Float>;

using Vec2 = glm::tvec2<Float>;
using Mat2 = glm::tmat2x2<Float>;


using String = std::string;

using IStream = std::istream;
using OStream = std::ostream;

using IFStream = std::ifstream;
using OFStream = std::ofstream;