#pragma once

#include "Component.h"

namespace cs
{
	class PhysicsBody
	{
	public:
		void GetAllColliderComponents(class PhysicsBodyComponent& body);
		// Test Collision against incoming body. The body that calls this method is the body that is effected
		void CompareBodies(PhysicsBodyComponent& body, const PhysicsBodyComponent& otherBody);
	};

	class PhysicsBodyComponent : public Component
	{
	public:
		friend PhysicsBody;

		bool disabled{ false };

		virtual void ImGuiDrawComponent() override;

	private:
		class Collider** colliders;
	};
}
