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
