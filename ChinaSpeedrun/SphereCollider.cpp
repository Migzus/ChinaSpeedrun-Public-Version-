#include "SphereCollider.h"

#include "imgui.h"
#include "Collider.h"
#include "PolygonCollider.h"
#include "PhysicsServer.h"
#include "Transform.h"
#include "Mathf.h"

cs::CollisionInfo cs::SphereColliderComponent::Intersect(const TransformComponent* transform, const ColliderComponent* otherCollider, const TransformComponent* otherTransform) const
{
	return otherCollider->Intersect(otherTransform, this, transform);
}

cs::CollisionInfo cs::SphereColliderComponent::Intersect(const TransformComponent* transform, const SphereColliderComponent* otherCollider, const TransformComponent* otherTransform) const
{
	// Sphere v Sphere
	return cs::collision_tests::SphereSphereIntersection(transform, this, otherTransform, otherCollider);
}

cs::CollisionInfo cs::SphereColliderComponent::Intersect(const TransformComponent* transform, const PolygonColliderComponent* otherCollider, const TransformComponent* otherTransform) const
{
	// Sphere v Polygon
	return cs::collision_tests::SpherePolygonIntersection(transform, this, otherTransform, otherCollider);
}

cs::CollisionInfo cs::SphereColliderComponent::Intersect(const TransformComponent* transform, const PlaneColliderComponent* otherCollider, const TransformComponent* otherTransform) const
{
	// Sphere v Plane
	return cs::collision_tests::SpherePlaneIntersection(transform, this, otherTransform, otherCollider);
}

void cs::SphereColliderComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Sphere Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat("Radius", &radius, 0.1f);
		ImGui::DragFloat("Friction", &friction, 0.01f);

		ImGui::TreePop();
	}
}
