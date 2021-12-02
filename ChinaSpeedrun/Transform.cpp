#include "Transform.h"

#include "imgui.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Mesh.h"

void cs::Transform::CalculateMatrix(TransformComponent& transform)
{
	transform.rotation = glm::radians(transform.rotationDegrees);
	transform.matrix = glm::translate(Matrix4x4(1.0f), transform.position) * glm::toMat4(Quaternion(transform.rotation)) * glm::scale(Matrix4x4(1.0f), transform.scale);
}

Matrix4x4& cs::Transform::GetMatrixTransform(TransformComponent& transform)
{
	return transform.matrix;
}

cs::TransformComponent::TransformComponent() :
	isQueued{ false }, updateTransform{ true }
{}

cs::TransformComponent::operator Matrix4x4& ()
{
	return matrix;
}

void cs::TransformComponent::SetPosition(const Vector3 pos)
{
	
}

void cs::TransformComponent::SetRotation(const Vector3 rot)
{

}

void cs::TransformComponent::SetRotationInDegrees(const Vector3 rotDeg)
{

}

void cs::TransformComponent::SetScale(const Vector3 scale)
{

}

void cs::TransformComponent::Init()
{
	if (!gameObject->HasComponent<MeshRendererComponent>())
		GenerateOBBExtents(gameObject->obb);
}

void cs::TransformComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat3("Position", &position.x, 0.1f);
		ImGui::DragFloat3("Rotation", &rotationDegrees.x);
		ImGui::DragFloat3("Scale", &scale.x);

		ImGui::TreePop();
	}
}

void cs::TransformComponent::GenerateOBBExtents(OBB& obb)
{
	obb = { Vector3(1.0f), Vector3(1.0f) };
}
