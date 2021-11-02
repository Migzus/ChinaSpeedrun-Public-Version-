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

		void QueueComponentUpdate(PhysicsComponent* pc); /*Potentially obsolete*/
		void QueueComponentCreate(PhysicsComponent* pc); /*Potentially obsolete*/
		void UpdateBody(PhysicsComponent* pc);
		void CreateBody(PhysicsComponent* pc);
		void DestroyBody(PhysicsComponent* pc);
	private:
		std::vector<PhysicsComponent*> componentToUpdate, componentToCreate; /*Potentially obsolete*/
		int velocityIterations, positionIterations;
		float frequency;
	};
}

