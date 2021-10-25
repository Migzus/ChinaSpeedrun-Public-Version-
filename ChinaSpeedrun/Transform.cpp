#include "Transform.h"

#include <glm/gtx/quaternion.hpp>
#include "imgui.h"

void cs::Transform::DrawComponent()
{

}

void cs::Transform::CalculateMatrix(TransformComponent& transform)
{
	transform.rotation = glm::radians(transform.rotationDegrees);
	transform.matrix = glm::translate(Matrix4x4(1.0f), transform.position) * glm::toMat4(Quaternion(transform.rotation)) * glm::scale(Matrix4x4(1.0f), transform.scale);
}

Matrix4x4& cs::Transform::GetMatrixTransform(TransformComponent& transform)
{
	return transform.matrix;
}

cs::TransformComponent::operator Matrix4x4& ()
{
	return matrix;
}

void cs::TransformComponent::ImGuiDrawComponent()
{
	/*
	Matrix4x4 _viewMatrix{ glm::inverse(Camera::GetViewMatrix(*CameraComponent::currentActiveCamera)) }, _projectionMatrix{ Camera::GetProjectionMatrix(*CameraComponent::currentActiveCamera) }, _identityMatrix{ Matrix4x4(1.0f) };

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();

	int _width, _height;
	renderer.GetViewportSize(_width, _height);

	ImGuizmo::SetRect(0.0f, 0.0f, static_cast<float>(_width), static_cast<float>(_height));
	ImGuizmo::DrawGrid(glm::value_ptr(_viewMatrix), glm::value_ptr(_projectionMatrix), glm::value_ptr(_identityMatrix), 100.0f);

	---------------------------------------------------------------------
	
	TransformComponent& _transform{ _activeObject->GetComponent<TransformComponent>() };
				Matrix4x4 _mainMatrix{ _transform };

				ImGuizmo::Manipulate(glm::value_ptr(_viewMatrix), glm::value_ptr(_projectionMatrix),
					ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(_mainMatrix));
				
				if (ImGuizmo::IsUsing())
				{
					_transform.position = _mainMatrix[3];
				}
	*/

	if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat3("Position", &position.x, 0.1f);
		ImGui::DragFloat3("Rotation", &rotationDegrees.x);
		ImGui::DragFloat3("Scale", &scale.x);

		ImGui::TreePop();
	}
}
