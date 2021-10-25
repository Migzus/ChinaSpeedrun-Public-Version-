#include "Rigidbody.h"

#include "Transform.h"
#include "PhysicsServer.h"
#include "Time.h"
#include "imgui.h"

void cs::Rigidbody::CalculatePhysics(RigidbodyComponent& rigidBody, TransformComponent& transform)
{
	rigidBody.velocity += (PhysicsServer::gravityDirection * rigidBody.gravity / rigidBody.mass) * Time::deltaTime;

	transform.position += rigidBody.velocity * Time::deltaTime;
}

void cs::RigidbodyComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Disabled", &disabled);
		ImGui::DragFloat("Mass", &mass, 0.01f);
		ImGui::DragFloat("Gravity", &gravity, 0.01f);

		ImGui::TreePop();
	}
}
