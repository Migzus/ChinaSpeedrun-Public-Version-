#pragma once

#include "Mathf.h"

#include <vector>

namespace cs
{
	struct CollisionInfo
	{
		bool valid{ false };
		Vector3 a{ 0.0f, 0.0f, 0.0f };
		Vector3 b{ 0.0f, 0.0f, 0.0f };
		Vector3 normal{ 0.0f, 0.0f, 0.0f };
		float length{ 0.0f };
		float errorLength{ 0.0f };
	};

	struct Collision
	{
		class PhysicsBodyComponent* bodyA;
		PhysicsBodyComponent* bodyB;
		CollisionInfo info;

		Collision(PhysicsBodyComponent* a, PhysicsBodyComponent* b, CollisionInfo newInfo);
	};

	struct RaycastHit
	{
		bool valid;
		float distance;
		Vector3 hitPosition, surfaceNormal;
	};

	class TransformComponent;
	class SphereColliderComponent;
	class PolygonColliderComponent;

	namespace collision_tests
	{
		CollisionInfo SphereSphereIntersection(const TransformComponent* t, const SphereColliderComponent* c, const TransformComponent* ot, const SphereColliderComponent* oc);
		CollisionInfo SpherePolygonIntersection(const TransformComponent* t, const SphereColliderComponent* c, const TransformComponent* ot, const PolygonColliderComponent* oc);
	}

	class PhysicsServer
	{
	public:
		constexpr static Vector3 gravityDirection{ 0.0f, -1.0f, 0.0f };
		constexpr static float airResistance{ 0.8f };

		std::vector<class PhysicsBodyComponent*> bodies;

		static RaycastHit Raycast(const Vector3 origin, const Vector3 direction, const float distance, const OBB& obb, const Matrix4x4& matrix);

		void Reset();

		void Step();
		void Test();
		// The solver, solves the queued collisions
		void Solve();

	private:
		std::vector<Collision> collisionQueue;

		static bool TestRayAgainstAxis(const Vector3 axis, const Vector3& delta, const Vector3& direction, float& minDistance, float& maxDistance, const float& obbMin, const float& obbMax);
	};
}
