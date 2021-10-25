#pragma once

namespace cs
{
	class Collider
	{
	public:
		virtual struct CollisionInfo Intersect(const Collider* collider) const = 0;
		virtual CollisionInfo Intersect(const class SphereCollider* collider) const = 0;
		virtual CollisionInfo Intersect(const class PolygonCollider* collider) const = 0;
	};
}
