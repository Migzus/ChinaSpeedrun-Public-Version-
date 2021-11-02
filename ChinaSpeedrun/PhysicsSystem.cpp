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
	for (auto c : componentToUpdate)
	{
		c->body->SetGravityScale(c->definition.gravityScale);
		c->body->SetLinearVelocity(c->definition.linearVelocity);
		c->body->SetAngularVelocity(c->definition.angularVelocity);
		c->body->SetAwake(c->definition.awake);
	}

	componentToUpdate.clear();

	for (auto c : componentToCreate)
	{
		auto& _tc(c->gameObject->GetComponentConst<TransformComponent>());
		Vector2 _position(_tc.position);
		float _angle(_tc.rotation.x);
		c->definition.angle = _angle;
		c->definition.position = { _position.x, _position.y };
		c->delta.Teleport(_position, _angle);

		if (c->body)
			world->DestroyBody(c->body);

		c->body = world->CreateBody(&c->definition);

		if (c->shape.shape)
			c->UpdateFixture();
	}

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
