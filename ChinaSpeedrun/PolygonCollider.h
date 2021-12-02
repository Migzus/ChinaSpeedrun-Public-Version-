#pragma once

#include "Collider.h"
#include "Mathf.h"

#include <vector>

namespace cs
{
	class Vertex;
	struct CollisionInfo;
	class Mesh;

	struct MeshCollider
	{
		const std::vector<Vertex>& vertices;
		const std::vector<uint32_t>& indices;

		MeshCollider(const std::vector<Vertex>& nVert, const std::vector<uint32_t>& nInd);
	};

	class PolygonColliderComponent : public ColliderComponent
	{
	public:
		float friction;

		PolygonColliderComponent();

		CollisionInfo Intersect(const class TransformComponent* transform, const ColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;
		CollisionInfo Intersect(const TransformComponent* transform, const class SphereColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;
		CollisionInfo Intersect(const TransformComponent* transform, const class PolygonColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;
		CollisionInfo Intersect(const TransformComponent* transform, const class PlaneColliderComponent* otherCollider, const TransformComponent* otherTransform) const override;

		virtual void Init() override;
		virtual void ImGuiDrawComponent() override;

		const MeshCollider& GetMeshCollider() const;

		~PolygonColliderComponent();

	private:
		MeshCollider* meshColldier;
	};
}
