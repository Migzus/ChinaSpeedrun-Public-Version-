#pragma once

#include "Collider.h"

namespace cs
{
	struct CollisionInfo;

	class SphereColliderComponent : public ColliderComponent
	{
	public:
		float radius{ 1.0f };
		float friction{ 0.0f };

		CollisionInfo Intersect(const class TransformComponent* transform, const ColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;
		CollisionInfo Intersect(const TransformComponent* transform, const class SphereColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;
		CollisionInfo Intersect(const TransformComponent* transform, const class PolygonColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;
		CollisionInfo Intersect(const TransformComponent* transform, const class PlaneColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;

		virtual void ImGuiDrawComponent() override;
	};
}
