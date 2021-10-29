#include "PhysicsLocator.h"

cs::PhysicsSystem* cs::PhysicsLocator::system;

cs::PhysicsSystem* cs::PhysicsLocator::GetPhysicsSystem()
{
	return system;
}

void cs::PhysicsLocator::Provide(PhysicsSystem* service)
{
	system = service;
}
