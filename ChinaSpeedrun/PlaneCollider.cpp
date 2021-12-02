#include "PlaneCollider.h"

#include "imgui.h"
#include "PhysicsServer.h"
#include "SphereCollider.h"
#include "PolygonCollider.h"

cs::CollisionInfo cs::PlaneColliderComponent::Intersect(const TransformComponent* transform, const ColliderComponent* otherCollider, const TransformComponent* otherTransform) const
{
    return otherCollider->Intersect(otherTransform, this, transform);
}

cs::CollisionInfo cs::PlaneColliderComponent::Intersect(const TransformComponent* transform, const SphereColliderComponent* otherCollider, const TransformComponent* otherTransform) const
{
	// Plane v Sphere
	return cs::collision_tests::SpherePlaneIntersection(otherTransform, otherCollider, transform, this);
}

cs::CollisionInfo cs::PlaneColliderComponent::Intersect(const TransformComponent* transform, const PolygonColliderComponent* otherCollider, const TransformComponent* otherTransform) const
{
	// Ignore Plane v Polygon
	return {};
}

cs::CollisionInfo cs::PlaneColliderComponent::Intersect(const TransformComponent* transform, const PlaneColliderComponent* otherCollider, const TransformComponent* otherTransform) const
{
	// Ignore Plane v Plane
	return {};
}

void cs::PlaneColliderComponent::Init()
{}

void cs::PlaneColliderComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Sphere Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat3("Normal", &plane.normal[0], 0.01f);
		ImGui::DragFloat("Length", &plane.length, 0.01f);

		ImGui::TreePop();
	}
}
