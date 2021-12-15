#pragma once

#include "Mathf.h"

namespace cs
{
	class CameraBase;

	class Camera
	{
	public:
		friend CameraBase;

		/**
		* Extract projection matrix from camera
		*/
		static const Matrix4x4& GetProjectionMatrix(CameraBase& camera);
		/**
		* Extract view matrix from camera
		*/
		static const Matrix4x4& GetViewMatrix(CameraBase& camera);
		static void UpdateCameraTransform(CameraBase& camera, class TransformComponent& transform);
		static void CalculateProjection(CameraBase& camera);
		static bool FrustumTest(const OBB& obb, const Matrix4x4& matrix);
		static Vector2 MouseToScreenSpace();
		static Vector3 MouseToWorldSpace();
	};
}
