#include "PhysicsSystem.h"

#include "Time.h"

#include <b2/b2_world.h>

void cs::PhysicsSystem::Update()
{
	world->Step(Time::fixedDeltaTime, velocityIterations, positionIterations);
}

cs::PhysicsSystem::PhysicsSystem() : world(new b2World({0.f, -9.81f})), velocityIterations(6), positionIterations(2)
{
	
}
