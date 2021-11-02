#pragma once

#include <b2/b2_body.h>

#include "Component.h"

#include "CollisionShape.h"
#include "Mathf.h"

namespace cs
{
	struct PhysicsDelta
	{
	public:
		// These can be removed
		Vector2 positionDifference;
		float angleDifference;
		void Step(const Vector2& newPosition, float newAngle);
		void Teleport(const Vector2& newPosition, float newAngle);
		PhysicsDelta();
	private:
		Vector2 positionPrevious;
		float anglePrevious;
	};

	class PhysicsComponent : public Component
	{
	public:
		b2BodyDef definition;
		b2Body* body;
		PhysicsDelta delta;
		CollisionShape shape;

		/*
		 * These might be obsolete once scripting is implemented
		 */
		void QueueForUpdate();
		void QueueForCreation();

		void UpdateFixtures();
		void DeleteFixtures();
		b2Shape* CreateDefaultShape() const;
		b2FixtureDef CreateDefaultFixtureDefinition() const;

		PhysicsComponent();
		~PhysicsComponent();

		void ImGuiDrawComponent() override;
	};
}
