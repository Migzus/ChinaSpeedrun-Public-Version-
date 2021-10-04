#pragma once

#include <entt/entt.hpp>

namespace cs
{
	class World
	{
	public:
		entt::registry registry;
		
		World();
		
		void Step();
	};
}