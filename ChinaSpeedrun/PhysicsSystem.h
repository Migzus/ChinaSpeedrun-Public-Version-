#pragma once
#include <vector>

class b2World;

namespace cs {
	class TransformComponent;
	class PhysicsComponent;

	class PhysicsSystem
	{
	public:
		b2World* world;
		int velocityIterations, positionIterations;
		void UpdateWorld();
		void UpdatePositions(PhysicsComponent& pc, TransformComponent& tc);
		PhysicsSystem();
	};
}

