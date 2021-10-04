#include "Transform.h"

void cs::Transform::CalculateMatrix(TransformComponent& transform)
{
	transform.matrix = glm::translate(glm::rotate(glm::rotate(glm::rotate(glm::scale(Matrix4x4(1.0f), transform.scale), transform.rotation.x, Vector3(0.0f, 1.0f, 0.0f)), transform.rotation.y, Vector3(1.0f, 0.0f, 0.0f)), transform.rotation.z, Vector3(0.0f, 0.0f, 1.0f)), transform.position);
}
