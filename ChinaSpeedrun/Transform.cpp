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

const Matrix4x4& cs::TransformComponent::GetMatrixTransform()
{
	return matrix;
}
