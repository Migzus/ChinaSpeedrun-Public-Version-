#pragma once

#include "Mathf.h"

namespace cs
{
	class Collider
	{
	public:
		virtual bool IntersectCollision();
	};

	class SphereCollider : public Collider
	{
	public:
		float radius;

		virtual bool IntersectCollision() override;
	};

	class PolygonCollider : public Collider
	{
	public:
		void CreateBasedOnMesh();
		virtual bool IntersectCollision() override;
	};

	class PhysicsBody
	{
	public:
		void GetAllColliderComponents();
		void CompareBodies();

	protected:
		Collider** collider;
	};

	class StaticBody : public PhysicsBody
	{

	};

	class RigidBody : public PhysicsBody
	{
	public:
		float mass;
		float airFriction;

		// This could actually be in a separate file, but since we're not
		// creating a full physics system we can just add them here...
		float gravity;
		Vector3 gravityDirection;

	private:
		Vector3 velocity;
	};
}
