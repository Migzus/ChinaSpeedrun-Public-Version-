#include "SphereCollider.h"

#include "imgui.h"

void cs::SphereColliderComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Sphere Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat("Radius", &radius, 0.1f);
		ImGui::DragFloat("Friction", &friction, 0.01f);

		ImGui::TreePop();
	}
}
