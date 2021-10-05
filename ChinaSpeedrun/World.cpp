#include "World.h"

#include <iostream>

#include "Transform.h"
#include "MeshRenderer.h"

cs::World::World()
{
	//entt::entity _entity{ registry.create() };
	//registry.emplace<CameraComponent>(_entity);
	//registry.emplace<TransformComponent>(_entity);
	//registry.emplace<MeshRendererComponent>(_entity);
}

void cs::World::Step()
{
	auto _objects{ registry.view<MeshRendererComponent, TransformComponent>() };
	for (auto e : _objects)
	{
		auto& _transform{ registry.get<TransformComponent>(e) };
		auto& _meshRenderer{ registry.get<MeshRendererComponent>(e) };

		Transform::CalculateMatrix(_transform);
		MeshRenderer::UpdateUBO(_meshRenderer, _transform);
	}
}
