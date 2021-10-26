#include "World.h"

#include <iostream>

#include "EngineEditor.h"

#include "Transform.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "CameraComponent.h"
#include "AudioSystem.h"
#include "AudioComponent.h"
#include "PhysicsServer.h"
#include "Rigidbody.h"

#include "GameObject.h"

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

void cs::World::Start()
{
	
}

void cs::World::Step()
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

		Camera::CalculatePerspective(_camera);
		Camera::UpdateCameraTransform(_camera, _transform);
	}

	auto _renderableObjects{ registry.view<MeshRendererComponent, TransformComponent>() };
	for (auto e : _renderableObjects)
	{
		auto& _transform{ registry.get<TransformComponent>(e) };
		auto& _meshRenderer{ registry.get<MeshRendererComponent>(e) };
		auto& _camera{ registry.get<CameraComponent>(_cameras.front()) };

		MeshRenderer::UpdateUBO(_meshRenderer, _transform, _camera);
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
