#include "Editor.h"

#include "Time.h"
#include "Input.h"
#include "Debug.h"

#include "World.h"
#include "PhysicsServer.h"
#include "ChinaEngine.h"
#include "Transform.h"
#include "GameObject.h"
#include "Camera.h"
#include "MeshRenderer.h"

cs::editor::EditorCamera::EditorCamera(EngineEditor* root) :
	editorRoot{ root }, position{ Vector3(0.0f) }, rotation{ Vector3(0.0f) }, movementsSpeed{ 10.0f }, rotationSpeed{ 0.002f }
{}

void cs::editor::EditorCamera::Update()
{
	if (Input::GetMouseHeld(1))
	{
		ScrollAdjustmentSpeed();
		RotateCamera();
		Movement();
	}

	if (Input::GetMousePressed(0) && !editorRoot->uiLayer->IsManipulating() && !editorRoot->uiLayer->IsInteractingWithWindow())
		SelectTest();

	// frustum testing
	auto _entities{ ChinaEngine::world.GetRegistry().view<TransformComponent, MeshRendererComponent>() };
	Matrix4x4 _pv{ proj * view };

	for (auto& e : _entities)
	{
		auto* _transform{ ChinaEngine::world.GetRegistry().try_get<TransformComponent>(e) };
		auto* _meshRenderer{ ChinaEngine::world.GetRegistry().try_get<MeshRendererComponent>(e) };

		if (_transform == nullptr || _meshRenderer == nullptr)
			return;

		_meshRenderer->visible = Camera::FrustumTest(_transform->gameObject->obb, _pv * Transform::GetMatrixTransform(*_transform));
	}
}

void cs::editor::EditorCamera::RotateCamera()
{
	rotation.y -= Input::mouseMovement.x * rotationSpeed;
	rotation.x -= Input::mouseMovement.y * rotationSpeed;

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
	Vector3 _mouseDirection{ World::MouseToWorldSpace() };
	bool _objectHit{ false };

	auto _transformView{ ChinaEngine::world.GetRegistry().view<TransformComponent>() };
	for (auto e : _transformView)
	{
		auto& _transformComponent{ ChinaEngine::world.GetRegistry().get<TransformComponent>(e) };
		RaycastHit _hit{ PhysicsServer::Raycast(position, _mouseDirection, farPlane, _transformComponent.gameObject->obb, Transform::GetMatrixTransform(_transformComponent)) };

		if (_hit.valid)
		{
			_objectHit = true;
			editorRoot->uiLayer->activeObject = _transformComponent.gameObject;
			break;
		}
	}

	if (!_objectHit)
		editorRoot->uiLayer->activeObject = nullptr;
}
