#include "EditorCamera.h"

#include "Time.h"
#include "Input.h"
#include "Debug.h"

#include <GLFW/glfw3.h>

cs::editor::EditorCamera::EditorCamera() :
	position{ Vector3(0.0f) }, rotation{ Vector3(0.0f) }, movementsSpeed{ 10.0f }, rotationSpeed{ 2.0f }
{}

void cs::editor::EditorCamera::Update()
{
	if (Input::GetMouseHeld(1))
	{
		ScrollAdjustmentSpeed();
		RotateCamera();
		Movement();
	}

	if (Input::GetMouseHeld(0))
	{
		SelectTest();
	}
}

void cs::editor::EditorCamera::RotateCamera()
{
	rotation.y -= Input::mouseMovement.x * rotationSpeed * Time::deltaTime;
	rotation.x -= Input::mouseMovement.y * rotationSpeed * Time::deltaTime;

	Mathf::Clamp(rotation.x, -Mathf::PI_2, Mathf::PI_2);
}

void cs::editor::EditorCamera::Movement()
{
	Vector3 _moveInput{ Vector3(
			Input::GetActionHeld("editor_right") - Input::GetActionHeld("editor_left"),
			Input::GetActionHeld("editor_up") - Input::GetActionHeld("editor_down"),
			Input::GetActionHeld("editor_backward") - Input::GetActionHeld("editor_forward")) };
	Vector4 _moveDirection{ glm::toMat4(Quaternion(rotation)) * Vector4(_moveInput, 1.0f) };

	position += Vector3(_moveDirection.x, _moveDirection.y, _moveDirection.z) * movementsSpeed * Time::deltaTime;
	view = glm::inverse(glm::translate(Matrix4x4(1.0f), position) * glm::toMat4(Quaternion(rotation)));
}

void cs::editor::EditorCamera::ScrollAdjustmentSpeed()
{
	movementsSpeed += Input::scrollOffset.y * 2.0f;
	Mathf::Clamp(movementsSpeed, 1.0f, 60.0f);
}

void cs::editor::EditorCamera::SelectTest()
{
	Debug::LogInfo("select");
}
