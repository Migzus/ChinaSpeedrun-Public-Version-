#pragma once

#include "Mathf.h"
#include "PhysicsBody.h"

namespace cs
{
	class Rigidbody : public PhysicsBody
	{
	public:
		void CalculatePhysics(class RigidbodyComponent& rigidBody, class TransformComponent& transform);
	};

	class RigidbodyComponent : public PhysicsBodyComponent
	{
	public:
		friend Rigidbody;

		float mass{ 0.1f };
		float gravity{ 9.81f };

		Vector3 velocity;

		virtual void ImGuiDrawComponent() override;
	};
}
