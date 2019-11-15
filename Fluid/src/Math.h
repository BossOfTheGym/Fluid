#ifndef MMMM_H
#define MMMM_H

#include <glm/common.hpp>
#include <glm/glm.hpp>
//#include <glm/matrix.hpp>


const float EPS = 1e-6f;
const float SQ2 = std::sqrt(2.0f);
const float SQ3 = std::sqrt(3.0f);

const float PI   = 3.141592653f;
const float PI2  = 2 * PI;
const float PI_2 = PI / 2;
const float PI_4 = PI / 4;
const float PI_5 = PI / 5;
const float PI_6 = PI / 6;
const float PI_8 = PI / 8;


using Vec2  = glm::vec2;
using Vec3  = glm::vec3;
using Vec4  = glm::vec4;
using Vec3i = glm::ivec3;

using Mat3 = glm::mat3;
using Mat4 = glm::mat4;

#endif