#pragma once

#include <entt/entt.hpp>

namespace cs {

	class World
	{
	public:
		World();
		void Step();
		entt::registry registry;
		struct CameraComponent& GetCameraComponent();
	};

}