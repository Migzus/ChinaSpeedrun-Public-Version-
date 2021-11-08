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

#include <glm/gtx/quaternion.hpp>

typedef glm::vec1 Vector1;
typedef glm::vec2 Vector2;
typedef glm::vec3 Vector3;
typedef glm::vec4 Vector4;

typedef glm::mat3 Matrix3x3;
typedef glm::mat4 Matrix4x4;

typedef glm::quat Quaternion;

namespace cs
{
	struct OBB
	{
		Vector3 minExtent{ Vector3(0.0f) }, maxExtent{ Vector3(0.0f) };
	};

	class Mathf
	{
	public:
		constexpr static float PI_2{ (float)M_PI_2 };
		constexpr static float PI{ (float)M_PI };
		constexpr static float TAU{ (float)M_PI * 2.0f };
		constexpr static float E{ (float)M_E };
		constexpr static float DEG2RAD{ (float)PI / 180.0f };
		constexpr static float RAD2DEG{ (float)180.0f / PI };

		static void InitRand();
		static int Rand();
		static float RandRange(const float min, const float max);
		// Clamps the value within the specified range
		template<class T>
		static void Clamp(T& value, T min, T max);
		// Clamps the value back to the other end of the specified range, pluss excess value is returned
		template<class T>
		static void LoopClamp(T& value, T min, T max);
		static float Project(const Vector3 projVec, const struct Plane plane);
		static Vector3 Project(const Vector3 projVec, const Vector3 alignVec);
		static float Magnitude(const Vector3 vec);
		static Vector3 CrossProduct(const Vector3 vec1, const Vector3 vec2);
		static float DotProduct(const Vector3 vec1, const Vector3 vec2);
		static float Max(const float v1, const float v2);
		static float Min(const float v1, const float v2);

		static void DecomposeMatrix(const Matrix4x4& transform, Vector3& position, Vector3& rotation, Vector3& scale);
	};
	
	template<class T>
	inline void Mathf::Clamp(T& value, T min, T max)
	{
		value =
			max * (value > max) +
			min * (value < min) +
			value * (value >= min && value <= max);
	}
	
	template<class T>
	inline void Mathf::LoopClamp(T& value, T min, T max)
	{
		value =
			(value - max) * (value > max) +
			(value - min + max) * (value < min) +
			value * (value >= min && value <= max);
	}
}
