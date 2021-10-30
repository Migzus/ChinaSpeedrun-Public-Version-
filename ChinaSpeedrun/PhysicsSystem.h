#pragma once

class b2World;

namespace cs {
	class PhysicsSystem
	{
	public:
		b2World* world;
		int velocityIterations, positionIterations;
		void Update();
		PhysicsSystem();
	};
}

