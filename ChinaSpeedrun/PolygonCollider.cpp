#include "PolygonCollider.h"

#include "imgui.h"
#include "Collider.h"
#include "SphereCollider.h"
#include "PhysicsServer.h"
#include "Transform.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Mesh.h"

void cs::PolygonColliderComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Polygon Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Use Mesh", &useMesh);
		if (ImGui::IsItemClicked() && useMesh)
			CreateBasedOnMesh();
		ImGui::DragFloat("Friction", &friction, 0.01f);

		ImGui::TreePop();
	}
}

cs::PolygonColliderComponent::PolygonColliderComponent()
{
	planes.push_back({ glm::normalize(Vector3(0.0f, -1.0f, 1.0f)), 0.0f });
	planes.push_back({ glm::normalize(Vector3(0.0f, -1.0f, 0.0f)), 0.0f });
}

// This method has yet to be tested
void cs::PolygonColliderComponent::CreateBasedOnMesh()
{
	if (!gameObject->HasComponent<MeshRendererComponent>())
	{
		Debug::LogWarning("Cannot create a polygon collider. No mesh was found on the same gameobject.");
		return;
	}

	planes.clear();

	Mesh* _mesh{ gameObject->GetComponent<MeshRendererComponent>().mesh };
	const auto& _indices{ _mesh->GetIndices() };
	const auto& _vertices{ _mesh->GetVertices() };
	const size_t _size{ _indices.size() / 3 }; // per triangle

	// could do this to be safe, but ... we don't have to
	// if ((_indices.size() & 3) == 0)

	for (size_t i{ 0 }; i < _size; i++)
	{
		Vector3 _normal{ Mathf::CrossProduct(_vertices[_indices[i]].position - _vertices[_indices[i + 1]].position, _vertices[_indices[i + 2]].position - _vertices[_indices[i + 1]].position) };
	
		planes.push_back({-glm::normalize(_normal), glm::length(_vertices[_indices[i + 1]].position)});
	}
}

const std::vector<cs::Plane>& cs::PolygonColliderComponent::GetPlanes() const
{
	return planes;
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
