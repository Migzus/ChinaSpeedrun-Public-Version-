#pragma once

#include "Component.h"

namespace cs
{
	struct PhysicsObject
	{
		class TransformComponent* transform;
		class ColliderComponent* collider;
	};

	class PhysicsBody
	{
	public:
		static void GetAllColliderComponents(class PhysicsBodyComponent* body);
		// Test Collision against incoming body. The body that calls this method is the body that is effected
		static struct CollisionInfo CompareBodies(PhysicsBodyComponent* body, const PhysicsBodyComponent* otherBody);
		static CollisionInfo CompareBodies(class RigidbodyComponent* body, const class StaticBodyComponent* otherBody);
		static CollisionInfo CompareBodies(StaticBodyComponent* body, const RigidbodyComponent* otherBody);
	};

	class PhysicsBodyComponent : public Component
	{
	public:
		friend PhysicsBody;

		enum class BodyType
		{
			UNDEFINED,
			STATIC,
			RIGID,
			KINEMATIC,
			AREA // Area is the equvalent of trigger in unity
		} bodyType;

		bool disabled;

		PhysicsBodyComponent();

		virtual void ImGuiDrawComponent() override;

	private:
		PhysicsObject* colliders;
	};
}
