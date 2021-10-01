#include "World.h"

#include <iostream>

#include "CameraComponent.h"
#include "CameraSystem.h"
#include "TransformComponent.h"
#include "MovementComponent.h"
#include "MovementSystem.h"
#include "MeshRenderer.h"
#include "MeshRendererSystem.h"
#include "AudioComponent.h"
#include "AudioSystem.h"

World::World() : audioSystem{ new AudioSystem } {

	/*entt::entity _entity = registry.create();
	registry.emplace<CameraComponent>(_entity);
	registry.emplace<TransformComponent>(_entity);
	registry.emplace<MovementComponent>(_entity);*/
}

void World::Step()
{
	auto _view = registry.view<cs::MeshRenderer, TransformComponent>();
	for (auto _entity : _view) {
		MeshRendererSystem::UpdatePosition(
			_view.get<cs::MeshRenderer>(_entity),
			_view.get<TransformComponent>(_entity));
	}

	auto _aview = registry.view<AudioComponent>();
	for (auto _entity : _aview) {
		// This actally plays the thing over and over and over and over...
		//auto _ac{ registry.get<AudioComponent>(_entity) };
		audioSystem->UpdatePlay(_aview.get<AudioComponent>(_entity));
	}

	auto _actcView = registry.view<AudioComponent, TransformComponent>();
	for (auto _entity : _actcView) {
		audioSystem->UpdateLocation(
			_actcView.get<AudioComponent>(_entity),
			_actcView.get<TransformComponent>(_entity));
	}

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