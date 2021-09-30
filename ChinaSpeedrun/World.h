#pragma once

#include <entt/entt.hpp>

class World
{
public:
	World();
	void Step();
	entt::registry registry;
};