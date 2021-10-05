#pragma once

#include "Mathf.h"

namespace cs
{
	class Camera
	{
	public:
		static const Matrix4x4& GetProjectionMatrix(class CameraComponent& camera);
		static const Matrix4x4& GetViewMatrix(CameraComponent& camera);
		static void UpdateCameraTransform(CameraComponent& camera, class TransformComponent& transform);
		static void CalculatePerspective(CameraComponent& camera);
	};
}
