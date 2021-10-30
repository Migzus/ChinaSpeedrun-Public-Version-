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
		void UpdateWorld();
		void UpdatePositions(PhysicsComponent& pc, TransformComponent& tc);
		void UpdateComponents();
		PhysicsSystem();

		void QueueComponentUpdate(PhysicsComponent* pc);
		void QueueComponentCreate(PhysicsComponent* pc);
	private:
		std::vector<PhysicsComponent*> componentToUpdate, componentToCreate;
		int velocityIterations, positionIterations;
	};
}

