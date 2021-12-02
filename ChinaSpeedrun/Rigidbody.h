#pragma once

#include "Mathf.h"
#include "PhysicsBody.h"

namespace cs
{
	class Rigidbody : public PhysicsBody
	{
	public:
		static void CalculatePhysics(class RigidbodyComponent& rigidBody, class TransformComponent& transform);
	};

	class RigidbodyComponent : public PhysicsBodyComponent
	{
	public:
		friend Rigidbody;

		float mass;
		float gravity;
		float endTime;
		Vector3 velocity, startForce;

		RigidbodyComponent();

		void ShootStartVelocity();
		void AddForce(const Vector3 additionalForce);
		virtual void ImGuiDrawComponent() override;

	private:
		Vector3 force;

		void UpdateTrajectory();
	};
}
