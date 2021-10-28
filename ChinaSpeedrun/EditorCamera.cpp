#include "EditorCamera.h"

#include <GLFW/glfw3.h>
#include <glm/gtx/quaternion.hpp>

#include "Time.h"

void cs::editor::EditorCamera::Update()
{
	Mathf::Clamp(rotation.y, -179.9f, 179.9f);
	Matrix4x4 _rotation{ glm::toMat4(Quaternion(rotation)) };
	
	//position += glm::translate(_rotation, Vector3(1.0f, 0.0f, 0.0f) * Time::deltaTime);
	view = glm::translate(Matrix4x4(1.0f), position) * _rotation;
}
