#pragma once

#include "Mathf.h"

#include <vector>

namespace cs
{
	struct CollisionInfo
	{
		bool valid{ false };
		Vector3 a;
		Vector3 b;
		Vector3 normal;
		float distance;
	};

	class PhysicsServer
	{
	public:
		constexpr static Vector3 gravityDirection{ 0.0f, 1.0f, 0.0f };

		void Step();
		// The solver, solves the queued collisions
		void Solve();

	private:
		std::vector<CollisionInfo> collisionQueue;
	};
}
