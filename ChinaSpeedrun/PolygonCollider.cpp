#include "PolygonCollider.h"

#include "imgui.h"
#include "Collider.h"
#include "SphereCollider.h"
#include "PhysicsServer.h"
#include "Transform.h"

void cs::PolygonColliderComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Polygon Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Use Mesh", &useMesh);
		ImGui::DragFloat("Friction", &friction, 0.01f);

		ImGui::TreePop();
	}
}

void cs::PolygonColliderComponent::CreateBasedOnMesh()
{

}

cs::CollisionInfo cs::PolygonColliderComponent::Intersect(const TransformComponent* transform, const ColliderComponent* otherCollider, const TransformComponent* otherTransform) const
{
	return otherCollider->Intersect(otherTransform, this, transform);
}

cs::CollisionInfo cs::PolygonColliderComponent::Intersect(const TransformComponent* transform, const SphereColliderComponent* otherCollider, const TransformComponent* otherTransform) const
{
	// Polygon v Sphere
	return cs::collision_tests::SpherePolygonIntersection(otherTransform, otherCollider, transform, this);
}

cs::CollisionInfo cs::PolygonColliderComponent::Intersect(const TransformComponent* transform, const PolygonColliderComponent* otherCollider, const TransformComponent* otherTransform) const
{
	// Ignore Polygon against Polygon (could get very expensive)
	return {};
}
