#include "World.h"

#include <iostream>

#include "Transform.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "CameraComponent.h"
#include "AudioSystem.h"
#include "AudioComponent.h"

cs::World::World() : audioSystem{ new AudioSystem }
{
	//entt::entity _entity{ registry.create() };
	//registry.emplace<CameraComponent>(_entity);
	//registry.emplace<TransformComponent>(_entity);
	//registry.emplace<MeshRendererComponent>(_entity);
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
}
