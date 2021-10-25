#include "StaticBody.h"

#include "imgui.h"

void cs::StaticBodyComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Static Body", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Disabled", &disabled);

		ImGui::TreePop();
	}
}
