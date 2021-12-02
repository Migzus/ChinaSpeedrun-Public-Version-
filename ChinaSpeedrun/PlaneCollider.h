#pragma once

#include "Collider.h"
#include "Mathf.h"

namespace cs
{
	struct CollisionInfo;

	struct Plane
	{
		Vector3 normal{ Vector3(0.0f, -1.0f, 0.0f) };
		float length{ 0.0f };
	};

	class PlaneColliderComponent : public ColliderComponent
	{
	public:
		Plane plane;

		CollisionInfo Intersect(const class TransformComponent* transform, const ColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;
		CollisionInfo Intersect(const TransformComponent* transform, const class SphereColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;
		CollisionInfo Intersect(const TransformComponent* transform, const class PolygonColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;
		CollisionInfo Intersect(const TransformComponent* transform, const class PlaneColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;

		virtual void Init() override;
		virtual void ImGuiDrawComponent() override;
	};
}
