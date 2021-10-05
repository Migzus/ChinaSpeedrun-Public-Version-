#include "World.h"

#include <iostream>

#include "Transform.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "CameraComponent.h"

cs::World::World()
{
	//entt::entity _entity{ registry.create() };
	//registry.emplace<CameraComponent>(_entity);
	//registry.emplace<TransformComponent>(_entity);
	//registry.emplace<MeshRendererComponent>(_entity);
}

void cs::World::Step()
{
	auto _cameras{ registry.view<CameraComponent, TransformComponent>() };
	for (auto e : _cameras)
	{
		auto& _transform{ registry.get<TransformComponent>(e) };
		auto& _camera{ registry.get<CameraComponent>(e) };

		Transform::CalculateMatrix(_transform);
		Camera::CalculatePerspective(_camera);
		Camera::UpdateCameraTransform(_camera, _transform);
	}

	auto _renderableObjects{ registry.view<MeshRendererComponent, TransformComponent>() };
	for (auto e : _renderableObjects)
	{
		auto& _transform{ registry.get<TransformComponent>(e) };
		auto& _meshRenderer{ registry.get<MeshRendererComponent>(e) };
		auto& _camera{ registry.get<CameraComponent>(_cameras.front()) };

		Transform::CalculateMatrix(_transform);
		MeshRenderer::UpdateUBO(_meshRenderer, _transform, _camera);
	}
}
