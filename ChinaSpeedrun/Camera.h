#pragma once

#include "Mathf.h"

namespace cs
{
	class CameraBase;

	class Camera
	{
	public:
		friend CameraBase;

		static const Matrix4x4& GetProjectionMatrix(CameraBase& camera);
		static const Matrix4x4& GetViewMatrix(CameraBase& camera);
		static void UpdateCameraTransform(CameraBase& camera, class TransformComponent& transform);
		static void CalculatePerspective(CameraBase& camera);
		static bool FrustumTest(const OBB& obb, const Matrix4x4& matrix);

	private:
		static bool Within(const float& a, const float& b, const float& c);
	};
}
