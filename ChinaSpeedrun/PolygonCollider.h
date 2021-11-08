#pragma once

#include "Collider.h"
#include "Mathf.h"

#include <vector>

namespace cs
{
	struct CollisionInfo;

	struct Plane
	{
		Vector3 normal;
		float length;
	};

	class PolygonColliderComponent : public ColliderComponent
	{
	public:
		bool useMesh{ false };
		float friction{ 0.0f };

		PolygonColliderComponent();

		void CreateBasedOnMesh();
		const std::vector<Plane>& GetPlanes() const;

		CollisionInfo Intersect(const class TransformComponent* transform, const ColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;
		CollisionInfo Intersect(const TransformComponent* transform, const class SphereColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;
		CollisionInfo Intersect(const TransformComponent* transform, const class PolygonColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;

		virtual void ImGuiDrawComponent() override;

	private:
		std::vector<Plane> planes;
	};
}
