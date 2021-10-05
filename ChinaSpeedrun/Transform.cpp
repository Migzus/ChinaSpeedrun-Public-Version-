#include "Transform.h"

void cs::Transform::CalculateMatrix(TransformComponent& transform)
{
	transform.matrix = glm::scale(glm::rotate(glm::rotate(glm::rotate(glm::translate(Matrix4x4(1.0f), transform.position), transform.rotation.z, Vector3(0.0f, 0.0f, 1.0f)), transform.rotation.y, Vector3(0.0f, 1.0f, 0.0f)), transform.rotation.x, Vector3(1.0f, 0.0f, 0.0f)), transform.scale);
}

const Matrix4x4& cs::TransformComponent::GetMatrixTransform()
{
	return matrix;
}
