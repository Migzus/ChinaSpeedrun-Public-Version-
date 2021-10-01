#pragma once

#include <entt/entt.hpp>

class AudioSystem;

class World
{
public:
	World();
	void Step();
	entt::registry registry;
	AudioSystem* audioSystem;
};