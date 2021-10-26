#pragma once

#include "Collider.h"
#include "Mathf.h"

#include <vector>

namespace cs
{
	struct PlanePart
	{
		Vector3 normal;
		float length;
	};

	class PolygonColliderComponent : public ColliderComponent
	{
	public:
		bool useMesh{ false };
		float friction{ 0.0f };

		void CreateBasedOnMesh();

		struct CollisionInfo Intersect(const class TransformComponent* transform, const ColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;
		CollisionInfo Intersect(const TransformComponent* transform, const class SphereColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;
		CollisionInfo Intersect(const TransformComponent* transform, const class PolygonColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;

		virtual void ImGuiDrawComponent() override;

	private:
		std::vector<PlanePart> planes;
	};
}
