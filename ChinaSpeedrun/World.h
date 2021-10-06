#pragma once

#include <entt/entt.hpp>

namespace cs
{
	class World
	{
	public:
		entt::registry registry;
		class AudioSystem* audioSystem;
		
		World();
		
		void Step();
	};
}