#include "PolygonCollider.h"

#include "imgui.h"
#include "Collider.h"
#include "SphereCollider.h"
#include "PhysicsServer.h"
#include "PlaneCollider.h"
#include "Transform.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Vertex.h"

void cs::PolygonColliderComponent::Init()
{
	if (gameObject->HasComponent<MeshRendererComponent>())
	{
		Mesh* _meshRef{ gameObject->GetComponent<MeshRendererComponent>().mesh };

		meshColldier = new MeshCollider(_meshRef->GetVertices(), _meshRef->GetIndices());
	}
}

void cs::PolygonColliderComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Polygon Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat("Friction", &friction, 0.01f);

		ImGui::TreePop();
	}
}

const cs::MeshCollider& cs::PolygonColliderComponent::GetMeshCollider() const
{
	return *meshColldier;
}

cs::PolygonColliderComponent::~PolygonColliderComponent()
{
	delete meshColldier;
}

cs::PolygonColliderComponent::PolygonColliderComponent() :
	friction{ 0.0f }, meshColldier{ nullptr }
{}

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

cs::CollisionInfo cs::PolygonColliderComponent::Intersect(const TransformComponent* transform, const PlaneColliderComponent* otherCollider, const TransformComponent* otherTransform) const
{
	// Polygon v Plane (this will also be ignored)
	return {};
}

cs::MeshCollider::MeshCollider(const std::vector<Vertex>& nVert, const std::vector<uint32_t>& nInd) :
	vertices{ nVert }, indices{ nInd }
{}
