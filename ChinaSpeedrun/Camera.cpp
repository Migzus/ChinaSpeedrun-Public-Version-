#include "Camera.h"
#include "Transform.h"
#include "CameraComponent.h"
#include "ChinaEngine.h"
#include "EditorCamera.h"

const Matrix4x4& cs::Camera::GetProjectionMatrix(CameraBase& camera)
{
	return camera.proj;
}

const Matrix4x4& cs::Camera::GetViewMatrix(CameraBase& camera)
{
	return camera.view;
}

void cs::Camera::UpdateCameraTransform(CameraBase& camera, TransformComponent& transform)
{
	camera.view = glm::inverse(Transform::GetMatrixTransform(transform));
}

void cs::Camera::CalculatePerspective(CameraBase& camera)
{
	camera.proj = glm::perspective(glm::radians(camera.fov), ChinaEngine::AspectRatio(), camera.nearPlane, camera.farPlane);
	camera.proj[1][1] *= -1.0f;
}

bool cs::Camera::FrustumTest(const OBB& obb, const Matrix4x4& matrix)
{
	bool _isInside{ false };
	Vector4 _corners[8]
	{
		{ obb.minExtent.x, obb.minExtent.y, obb.minExtent.z, 1.0f },
		{ obb.maxExtent.x, obb.minExtent.y, obb.minExtent.z, 1.0f },
		{ obb.minExtent.x, obb.maxExtent.y, obb.minExtent.z, 1.0f },
		{ obb.maxExtent.x, obb.maxExtent.y, obb.minExtent.z, 1.0f },

		{ obb.minExtent.x, obb.minExtent.y, obb.maxExtent.z, 1.0f },
		{ obb.maxExtent.x, obb.minExtent.y, obb.maxExtent.z, 1.0f },
		{ obb.minExtent.x, obb.maxExtent.y, obb.maxExtent.z, 1.0f },
		{ obb.maxExtent.x, obb.maxExtent.y, obb.maxExtent.z, 1.0f }
	};

	for (size_t i{ 0 }; i < 8; i++)
	{
		Vector4 _corner{ matrix * _corners[i] }; // this operation becomes more expensive when adding more objects
		_isInside |= Within(-_corner.w, _corner.x, _corner.w) && Within(-_corner.w, _corner.y, _corner.w) && Within(0.0f, _corner.z, _corner.w);
	}

	return _isInside;
}

bool cs::Camera::Within(const float& a, const float& b, const float& c)
{
	return a <= b && b <= c;
}
