#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

typedef glm::vec1 Vector1;
typedef glm::vec2 Vector2;
typedef glm::vec3 Vector3;
typedef glm::vec4 Vector4;

typedef glm::mat3 Matrix3x3;
typedef glm::mat4 Matrix4x4;

typedef glm::quat Quaternion;

/*
namespace glm
{
	constexpr static Vector3 up{ Vector3(0.0f, 1.0f, 0.0f) };
}*/

namespace cs
{
	class Mathf
	{
	public:
		constexpr static float PI{ (float)M_PI };
		constexpr static float TAU{ (float)M_PI * 2.0f };

		// Clamps the value within the specified range
		static void Clamp(float& value, float min, float max);
		// Clamps the value back to the other end of the specified range, pluss excess value is returned
		static void LoopClamp(float& value, float min, float max);
	private:
	};
}
