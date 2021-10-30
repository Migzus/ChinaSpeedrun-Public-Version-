#pragma once

#include <b2/b2_body.h>

#include "Component.h"

#include "Mathf.h"

namespace cs
{
	struct PhysicsDelta
	{
	public:
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

		/*
		 * Make this dynamically changable in the future
		 */
		b2CircleShape* shape;

		PhysicsComponent();
		~PhysicsComponent();

		void ImGuiDrawComponent() override;

	private:
		void QueueForUpdate();
		void QueueForCreation();
	};
}
