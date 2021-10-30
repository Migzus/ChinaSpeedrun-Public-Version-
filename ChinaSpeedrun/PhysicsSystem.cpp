#include "PhysicsSystem.h"

#include "Time.h"

#include <b2/b2_world.h>

#include "Debug.h"
#include "PhysicsComponent.h"
#include "PhysicsLocator.h"
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

void cs::PhysicsSystem::UpdateComponents()
{
	for (auto c : componentToUpdate)
	if (c->body)
	{
		c->body->SetTransform(c->definition.position, c->body->GetAngle());
		c->body->SetGravityScale(c->definition.gravityScale);
		c->body->SetLinearVelocity(c->definition.linearVelocity);
		c->body->SetAngularVelocity(c->definition.angularVelocity);
		c->body->SetAwake(c->definition.awake);
	}
	else
	{
		Debug::LogWarning("PhysicsComponent: Didn't update body, body doesn't exist");
	}

	componentToUpdate.clear();

	for (auto c : componentToCreate)
	{
		if (c->body) world->DestroyBody(c->body);
		c->body = world->CreateBody(&c->definition);
		c->body->CreateFixture((b2Shape*)c->shape, 1.f);
	}

	componentToCreate.clear();
}

cs::PhysicsSystem::PhysicsSystem() : world(new b2World({0.f, -9.81f})), velocityIterations(6), positionIterations(2)
{
	
}

void cs::PhysicsSystem::QueueComponentUpdate(PhysicsComponent* pc)
{
	componentToUpdate.push_back(pc);
}

void cs::PhysicsSystem::QueueComponentCreate(PhysicsComponent* pc)
{
	componentToCreate.push_back(pc);
}
