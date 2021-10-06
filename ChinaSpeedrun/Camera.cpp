#include "Camera.h"
#include "Transform.h"
#include "CameraComponent.h"
#include "ChinaEngine.h"

const Matrix4x4& cs::Camera::GetProjectionMatrix(CameraComponent& camera)
{
	return camera.proj;
}

const Matrix4x4& cs::Camera::GetViewMatrix(CameraComponent& camera)
{
	return camera.view;
}

void cs::Camera::UpdateCameraTransform(CameraComponent& camera, TransformComponent& transform)
{
	camera.view = transform;
}

void cs::Camera::CalculatePerspective(CameraComponent& camera)
{
	camera.proj = glm::perspective(glm::radians(camera.fov), ChinaEngine::AspectRatio(), camera.nearPlane, camera.farPlane);
}
