#pragma once

#include "Collider.h"
#include "Component.h"

namespace cs
{
	class SphereCollider : public Collider
	{
	public:
		struct CollisionInfo Intersect(const Collider* collider) const override;
		CollisionInfo Intersect(const SphereCollider* collider) const override;
		CollisionInfo Intersect(const PolygonCollider* collider) const override;
	};

	class SphereColliderComponent : public Component
	{
	public:
		friend SphereCollider;

		float radius{ 1.0f };
		float friction{ 0.0f };

		virtual void ImGuiDrawComponent() override;
	};
}
