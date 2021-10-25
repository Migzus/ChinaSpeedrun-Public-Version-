#pragma once

#include "Collider.h"
#include "Component.h"
#include "Mathf.h"

#include <vector>

namespace cs
{
	class PolygonCollider : public Collider
	{
	public:
		void CreateBasedOnMesh();
		struct CollisionInfo Intersect(const Collider* collider) const override;
		CollisionInfo Intersect(const SphereCollider* collider) const override;
		CollisionInfo Intersect(const PolygonCollider* collider) const override;
	};

	class PolygonColliderComponent : public Component
	{
	public:
		friend PolygonCollider;

		bool useMesh{ false };
		float friction{ 0.0f };

		virtual void ImGuiDrawComponent() override;

	private:
		std::vector<Vector3> points;
		std::vector<Vector3> normals;
	};
}
