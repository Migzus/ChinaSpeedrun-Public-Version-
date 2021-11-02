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
		void UpdateComponents();
		void UpdateWorld();
		void UpdatePositions(PhysicsComponent& pc, TransformComponent& tc);
		PhysicsSystem();

		void QueueComponentUpdate(PhysicsComponent* pc);
		void QueueComponentCreate(PhysicsComponent* pc);
		void UpdateBody(PhysicsComponent* pc);
		void CreateBody(PhysicsComponent* pc);
		void DestroyBody(PhysicsComponent* pc);
	private:
		std::vector<PhysicsComponent*> componentToUpdate, componentToCreate;
		int velocityIterations, positionIterations;
		float frequency;
	};
}

