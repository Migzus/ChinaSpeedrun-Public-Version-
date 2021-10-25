#include "PhysicsBody.h"

#include "imgui.h"

void cs::PhysicsBody::GetAllColliderComponents(PhysicsBodyComponent& body)
{

}

void cs::PhysicsBody::CompareBodies(PhysicsBodyComponent& body, const PhysicsBodyComponent& otherBody)
{

}

void cs::PhysicsBodyComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Physics Body", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Disabled", &disabled);

		ImGui::TreePop();
	}
}
