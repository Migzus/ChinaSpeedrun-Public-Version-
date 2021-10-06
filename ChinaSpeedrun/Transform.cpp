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
