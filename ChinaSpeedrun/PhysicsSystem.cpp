#include "PhysicsSystem.h"

#include "Time.h"

#include <b2/b2_world.h>

#include "PhysicsComponent.h"
#include "Transform.h"

void cs::PhysicsSystem::UpdateWorld()
{
	world->Step(Time::fixedDeltaTime, velocityIterations, positionIterations);
}

void cs::PhysicsSystem::UpdatePositions(PhysicsComponent& pc, TransformComponent& tc)
{
	if (pc.body)
	{
		auto _pos(pc.body->GetPosition());
		auto _ang(pc.body->GetAngle());
		pc.delta.Step({ _pos.x, _pos.y }, _ang);
		tc.position.x += pc.delta.positionDifference.x;
		tc.position.y += pc.delta.positionDifference.y;
		tc.rotation.x += pc.delta.angleDifference;
	}
}

cs::PhysicsSystem::PhysicsSystem() : world(new b2World({0.f, -9.81f})), velocityIterations(6), positionIterations(2)
{
	
}
