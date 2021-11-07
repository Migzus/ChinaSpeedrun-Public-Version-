#include "World.h"
/*
#include <iostream>

#include "Editor.h"

#include "VulkanEngineRenderer.h"
#include "Input.h"

#include "Transform.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "CameraComponent.h"
#include "AudioSystem.h"
#include "AudioComponent.h"
#include "PhysicsServer.h"
#include "Rigidbody.h"

#include "GameObject.h"

cs::CameraBase* cs::World::mainCamera;

cs::GameObject* cs::World::InstanceObject(const char* name, const Vector3 position, const Vector3 rotation, const Vector3 scale)
{
	GameObject* _newObject{ new GameObject };
	_newObject->entity = registry.create();
	_newObject->name = name;

	TransformComponent& _transform{ _newObject->AddComponent<TransformComponent>() };
	_transform.position = position;
	_transform.rotationDegrees = rotation;
	_transform.scale = scale;

	objects.push_back(_newObject);
	return _newObject;
}

cs::World::World() :
	physicsServer{ new PhysicsServer }, audioSystem{ new AudioSystem }
{}

Vector2 cs::World::MouseToScreenSpace()
{
	int _width{ 0 }, _height{ 0 };
	ChinaEngine::renderer.GetViewportSize(_width, _height);

	return { 2.0f * Input::mousePosition.x / (float)_width - 1.0f, 2.0f * Input::mousePosition.y / (float)_height - 1.0f };
}

Vector3 cs::World::MouseToWorldSpace()
{
	Vector4 _eyeCoords{ glm::inverse(Camera::GetProjectionMatrix(*mainCamera)) * Vector4(MouseToScreenSpace(), -1.0f, 1.0f) };

	_eyeCoords.z = -1.0f;
	_eyeCoords.w = 0.0f;

	return glm::normalize(Vector3(glm::inverse(Camera::GetViewMatrix(*mainCamera)) * _eyeCoords));
}

void cs::World::Start()
{
	auto _cameras{ registry.view<CameraComponent>() };

	if (_cameras.empty())
		Debug::LogError("No camera is active in the current scene.");

	for (auto e : _cameras)
	{
		mainCamera = &registry.get<CameraComponent>(e);
		Camera::CalculatePerspective(*mainCamera);
		break;
	}
}

void cs::World::Stop()
{
	
}

void cs::World::Step()
{
	switch (ChinaEngine::editor.GetPlaymodeState())
	{
	case cs::editor::EngineEditor::Playmode::EDITOR:
		StepEditor();
		break;
	case cs::editor::EngineEditor::Playmode::PLAY:
		StepEngine();
		break;
	case cs::editor::EngineEditor::Playmode::PAUSE:
		break;
	}
}

void cs::World::StepEngine()
{
	auto _audioComponentView{ registry.view<AudioComponent>() };
	for (auto e : _audioComponentView)
	{
		auto& _audioComponent{ registry.get<AudioComponent>(e) };
		audioSystem->Update(_audioComponent);
	}

	auto _transformComponentView{ registry.view<TransformComponent>() };
	for (auto e : _transformComponentView)
	{
		auto& _transformComponent{ registry.get<TransformComponent>(e) };
		Transform::CalculateMatrix(_transformComponent);
	}

	auto _cameras{ registry.view<CameraComponent, TransformComponent>() };
	for (auto e : _cameras)
	{
		auto& _transform{ registry.get<TransformComponent>(e) };
		auto& _camera{ registry.get<CameraComponent>(e) };

		Camera::UpdateCameraTransform(_camera, _transform);
	}

	auto _renderableObjects{ registry.view<MeshRendererComponent, TransformComponent>() };
	for (auto e : _renderableObjects)
	{
		auto& _transform{ registry.get<TransformComponent>(e) };
		auto& _meshRenderer{ registry.get<MeshRendererComponent>(e) };

		MeshRenderer::UpdateUBO(_meshRenderer, _transform, *World::mainCamera);
	}

	physicsServer->Step();

	auto _physicsSimulations{ registry.view<RigidbodyComponent, TransformComponent>() };
	for (auto e : _physicsSimulations)
	{
		auto& _transform{ registry.get<TransformComponent>(e) };
		auto& _rigidbody{ registry.get<RigidbodyComponent>(e) };

		Rigidbody::CalculatePhysics(_rigidbody, _transform);
	}
}

void cs::World::StepEditor()
{
	auto _transformComponentView{ registry.view<TransformComponent>() };
	for (auto e : _transformComponentView)
	{
		auto& _transformComponent{ registry.get<TransformComponent>(e) };
		Transform::CalculateMatrix(_transformComponent);
	}
	
	auto _renderableObjects{ registry.view<MeshRendererComponent, TransformComponent>() };
	for (auto e : _renderableObjects)
	{
		auto& _transform{ registry.get<TransformComponent>(e) };
		auto& _meshRenderer{ registry.get<MeshRendererComponent>(e) };

		MeshRenderer::UpdateUBO(_meshRenderer, _transform, *World::mainCamera);
	}
}

void cs::World::DeleteAllObjects()
{
	for (auto object : objects)
		delete object;
}

const uint64_t cs::World::GetUBONextOffset() const
{
	return UniformBufferObject::GetByteSize() * registry.size<MeshRendererComponent>();
}

const std::vector<cs::GameObject*>& cs::World::GetObjects()
{
	return objects;
}

entt::registry& cs::World::GetRegistry()
{
	return registry;
}
*/