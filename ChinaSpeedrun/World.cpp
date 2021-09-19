#include "World.h"

#include <iostream>

#include "CameraComponent.h"
#include "CameraSystem.h"
#include "TransformComponent.h"
#include "MovementComponent.h"
#include "MovementSystem.h"

cs::World::World() {
	/*entt::entity _entity = registry.create();
	registry.emplace<CameraComponent>(_entity);
	registry.emplace<TransformComponent>(_entity);
	registry.emplace<MovementComponent>(_entity);*/
}

void cs::World::Step()
{
	/*
	auto _group = registry.group<MovementComponent>(entt::get<TransformComponent>);
	for (auto _entity : _group) {
		auto& [_movement, _transform] = _group.get<MovementComponent, TransformComponent>(_entity);
		MovementSystem::HandleMovement(_movement, _transform);
	}
	
	for (int i = 0; i < 10; i++) {
		registry.emplace<Matrix4x4>(registry.create());
	}

	auto _entity = registry.create();
	registry.emplace<CameraComponent>(_entity);

	for (int i = 0; i < 10; i++) {
		registry.emplace<Matrix4x4>(registry.create());
	}

	registry.sort<CameraComponent, Matrix4x4>();

	entt::basic_view view = registry.view<CameraComponent>();

	CameraSystem system;

	for (auto& e : view) {
		const auto& camera = view.get<CameraComponent>(e);

	}*/
}