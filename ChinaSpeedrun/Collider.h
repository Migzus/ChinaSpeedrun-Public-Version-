#pragma once

#include "Component.h"

namespace cs
{
	class ColliderComponent : public Component
	{
	public:
		virtual void Init() override;

		virtual struct CollisionInfo Intersect(const class TransformComponent* transform, const ColliderComponent* otherCollider, const TransformComponent* otherTransform) const;
		virtual CollisionInfo Intersect(const TransformComponent* transform, const class SphereColliderComponent* otherCollider, const TransformComponent* otherTransform) const;
		virtual CollisionInfo Intersect(const TransformComponent* transform, const class PolygonColliderComponent* otherCollider, const TransformComponent* otherTransform) const;
		virtual CollisionInfo Intersect(const TransformComponent* transform, const class PlaneColliderComponent* otherCollider, const TransformComponent* otherTransform) const;

		virtual void ImGuiDrawComponent() override;
	};
}
