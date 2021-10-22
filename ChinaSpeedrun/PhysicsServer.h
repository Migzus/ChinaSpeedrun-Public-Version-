#pragma once

#include "Mathf.h"
#include "Transform.h"
#include "Component.h"

namespace cs
{
	struct CollisionInfo
	{
		bool valid{ false };
		Vector3 a;
		Vector3 b;
		Vector3 normal;
		float depth;
	};

	class Collider
	{
	public:
		virtual CollisionInfo Intersect(const Collider* collider) const = 0;
		virtual CollisionInfo Intersect(const class SphereCollider* collider) const = 0;
		virtual CollisionInfo Intersect(const class PolygonCollider* collider) const = 0;
	};

	class SphereCollider : public Collider
	{
	public:
		CollisionInfo Intersect(const Collider* collider) const override;
		CollisionInfo Intersect(const SphereCollider* collider) const override;
		CollisionInfo Intersect(const PolygonCollider* collider) const override;
	};

	class SphereColliderComponent : public Component
	{
	public:
		friend SphereCollider;

		float radius{ 1.0f };

		virtual void ImGuiDrawComponent() override;
	};

	class PolygonCollider : public Collider
	{
	public:
		void CreateBasedOnMesh();
		CollisionInfo Intersect(const Collider* collider) const override;
		CollisionInfo Intersect(const SphereCollider* collider) const override;
		CollisionInfo Intersect(const PolygonCollider* collider) const override;
	};

	class PolygonColliderComponent : public Component
	{
	public:
		friend PolygonCollider;

	private:
		std::vector<Vector3> points;
		std::vector<Vector3> normals;
	};

	class PhysicsBody
	{
	public:
		void GetAllColliderComponents(class PhysicsBodyComponent& body);
		// Test Collision against incoming body. The body that calls this method is the body that is effected
		void CompareBodies(PhysicsBodyComponent& body, const class PhysicsBodyComponent& otherBody);
	};

	class PhysicsBodyComponent : public Component
	{
	public:
		friend PhysicsBody;

		bool disabled{ false };

		virtual void ImGuiDrawComponent() override;
	
	private:
		Collider** colliders;
	};

	class StaticBody : public PhysicsBody
	{

	};

	class StaticBodyComponent : public PhysicsBodyComponent
	{
	public:
		friend StaticBody;

		virtual void ImGuiDrawComponent() override;
	};

	class RigidBody : public PhysicsBody
	{
	public:
		void CalculatePhysics(class RigidBodyComponent& rigidBody, TransformComponent& transform);
	};

	class RigidBodyComponent : public PhysicsBodyComponent
	{
	public:
		friend RigidBody;

		float mass{ 0.1f };
		float airFriction;

		// This could actually be in a separate file, but since we're not
		// creating a full physics system we can just add them here...
		float gravity;
		Vector3 gravityDirection;

		Vector3 velocity;

		virtual void ImGuiDrawComponent() override;
	};

	class PhysicsServer
	{
	public:
		void Step();
		void Solve();
	};
}
