#include "Transform.h"

#include "imgui.h"
#include "GameObject.h"
#include "MeshRenderer.h"

void cs::Transform::CalculateMatrix(TransformComponent& transform)
{
	//transform.rotation = glm::radians(transform.rotationDegrees);
	transform.matrix = glm::translate(Matrix4x4(1.0f), transform.globalPosition) * glm::toMat4(Quaternion(transform.globalRotation)) * glm::scale(Matrix4x4(1.0f), transform.globalScale);
}

void cs::Transform::CalculateMatrix(TransformComponent& transform, Matrix4x4& matrixOffset)
{
	transform.matrix = matrixOffset * glm::translate(Matrix4x4(1.0f), transform.globalPosition) * glm::toMat4(Quaternion(transform.globalRotation)) * glm::scale(Matrix4x4(1.0f), transform.globalScale);
}

Matrix4x4& cs::Transform::GetMatrixTransform(TransformComponent& transform)
{
	return transform.matrix;
}

cs::TransformComponent::TransformComponent() :
	updateTransform{ true }
{}

cs::TransformComponent::operator Matrix4x4&()
{
	return matrix;
}

cs::TransformComponent& cs::TransformComponent::operator+=(const TransformComponent& transform)
{
	Translate(transform.globalPosition);
	Rotate(transform.globalRotation);
	Scale(transform.globalScale);

	return *this;
}

void cs::TransformComponent::Translate(const Vector3& translation)
{
	globalPosition += translation;
	updateTransform = true;
}

void cs::TransformComponent::Rotate(const Vector3& rotate)
{
	globalRotation += rotate;
	updateTransform = true;
}

void cs::TransformComponent::Scale(const Vector3& scale)
{
	globalScale += scale;
	updateTransform = true;
}

void cs::TransformComponent::SetPosition(const Vector3& pos)
{
	position = pos;
	updateTransform = true;
}

void cs::TransformComponent::SetRotation(const Vector3& rot)
{
	rotation = rot;
	updateTransform = true;
}

void cs::TransformComponent::SetRotationInDegrees(const Vector3& rotDeg)
{
	rotationDegrees = rotDeg;
	updateTransform = true;
}

void cs::TransformComponent::SetScale(const Vector3& scale)
{
	this->scale = scale;
	updateTransform = true;
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
		Vector3 _position{ position }, _scale{ scale };

		if (ImGui::DragFloat3("Position", &_position.x, 0.1f))
			SetPosition(_position);
		if (ImGui::DragFloat3("Rotation", &rotationDegrees.x))
			SetRotation(rotationDegrees * Mathf::DEG2RAD);
		if (ImGui::DragFloat3("Scale", &_scale.x))
			SetScale(_scale);

		ImGui::TreePop();
	}
}

void cs::TransformComponent::GenerateOBBExtents(OBB& obb)
{
	obb = { Vector3(1.0f), Vector3(1.0f) };
}

void cs::TransformComponent::RootTraverseChildren()
{
	if (updateTransform)
	{
		UpdateGlobalTransform();

		Matrix4x4 _world{ gameObject->HasParent() && gameObject->GetParent()->HasComponent<TransformComponent>() ? gameObject->GetParent()->GetComponent<TransformComponent>() : Matrix4x4(1.0f) };

		Transform::CalculateMatrix(*this);

		for (GameObject* childObject : gameObject->GetChildren())
			if (childObject->HasComponent<TransformComponent>())
				childObject->GetComponent<TransformComponent>().TraverseChildren(*this, updateTransform);

		updateTransform = false;
	}
}

void cs::TransformComponent::UpdateGlobalTransform()
{
	// these global variables aren't fully representing the acutal real values
	// E.G. position will not be affected by parent rotation (would implement that in the future)
	globalPosition = position;
	globalRotation = rotation;
	globalScale = scale;
}

void cs::TransformComponent::TraverseChildren(TransformComponent& transform, bool& dirty)
{
	dirty |= updateTransform;

	if (dirty)
	{
		UpdateGlobalTransform();
		Transform::CalculateMatrix(*this, transform);
		updateTransform = false;

		for (GameObject* childObject : gameObject->GetChildren())
			if (childObject->HasComponent<TransformComponent>())
				childObject->GetComponent<TransformComponent>().TraverseChildren(*this, dirty);
	}
}
