#include "Camera.h"

#include "Transform.h"
#include "CameraComponent.h"
#include "ChinaEngine.h"
#include "VulkanEngineRenderer.h"
#include "Input.h"
#include "SceneManager.h"
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

void cs::Camera::CalculateProjection(CameraBase& camera)
{
	switch (camera.projection)
	{
	case CameraComponent::Projection::ORTHOGRAPHIC:
		// we might want to multiply aspect ratio into this somewhere, or just pass window size directly
		camera.proj = glm::ortho(camera.leftPlane, camera.rightPlane, camera.bottomPlane, camera.topPlane, camera.nearPlane, camera.farPlane);
		break;
	case CameraComponent::Projection::PERSPECTIVE:
		camera.proj = glm::perspective(glm::radians(camera.fov), ChinaEngine::AspectRatio(), camera.nearPlane, camera.farPlane);
		break;
	}
	
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
		Vector4 _corner{ matrix * _corners[i] };
		_isInside |= Mathf::Within(-_corner.w, _corner.x, _corner.w) && Mathf::Within(-_corner.w, _corner.y, _corner.w) && Mathf::Within(0.0f, _corner.z, _corner.w);
	}

	return _isInside;
}

Vector2 cs::Camera::MouseToScreenSpace()
{
	int _width{ 0 }, _height{ 0 };
	ChinaEngine::renderer.GetViewportSize(_width, _height);

	return { 2.0f * Input::mousePosition.x / (float)_width - 1.0f, 2.0f * Input::mousePosition.y / (float)_height - 1.0f };
}

Vector3 cs::Camera::MouseToWorldSpace()
{
	Vector4 _eyeCoords{ glm::inverse(Camera::GetProjectionMatrix(*SceneManager::mainCamera)) * Vector4(MouseToScreenSpace(), -1.0f, 1.0f) };

	_eyeCoords.z = -1.0f;
	_eyeCoords.w = 0.0f;

	return glm::normalize(Vector3(glm::inverse(Camera::GetViewMatrix(*SceneManager::mainCamera)) * _eyeCoords));
}
