#include "PhysicsSystem.h"

#include <b2/b2_world.h>

#include "Debug.h"
#include "GameObject.h"
#include "PhysicsComponent.h"
#include "PhysicsLocator.h"
#include "Transform.h"

void cs::PhysicsSystem::UpdateWorld()
{
	world->Step(frequency, velocityIterations, positionIterations);
}

void cs::PhysicsSystem::UpdatePositions(PhysicsComponent& pc, TransformComponent& tc)
{
	/*
	 * Body should always exist when it needs to be updated
	 * When the implementation is done this check should not be necessary anymore
	 */
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
	/*
	 * Updating components this way probably causes cache misses.
	 * Though this is the exception not the norm, so it should be fine.
	 */
	for (auto pc : componentToUpdate)
		UpdateBody(pc);

	componentToUpdate.clear();

	for (auto pc : componentToCreate)
		CreateBody(pc);

	componentToCreate.clear();
}

cs::PhysicsSystem::PhysicsSystem() : world(new b2World({0.f, -9.81f})), velocityIterations(6), positionIterations(2), frequency(1.f/50.f)
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

void cs::PhysicsSystem::UpdateBody(PhysicsComponent* pc)
{
	pc->body->SetGravityScale(pc->definition.gravityScale);
	pc->body->SetLinearVelocity(pc->definition.linearVelocity);
	pc->body->SetAngularVelocity(pc->definition.angularVelocity);
	pc->body->SetAwake(pc->definition.awake);
}

void cs::PhysicsSystem::CreateBody(PhysicsComponent* pc)
{
	auto& _tc(pc->gameObject->GetComponentConst<TransformComponent>());
	Vector2 _position(_tc.position);
	float _angle(_tc.rotation.x);
	pc->definition.angle = _angle;
	pc->definition.position = { _position.x, _position.y };
	pc->delta.Teleport(_position, _angle);

	DestroyBody(pc);
	pc->body = world->CreateBody(&pc->definition);
	pc->UpdateFixtures();
}

void cs::PhysicsSystem::DestroyBody(PhysicsComponent* pc)
{
	if (pc->body)
	{
		world->DestroyBody(pc->body);
		pc->body = nullptr;
	}
}
