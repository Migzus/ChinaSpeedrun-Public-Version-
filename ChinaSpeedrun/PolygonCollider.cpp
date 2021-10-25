#include "PolygonCollider.h"

#include "imgui.h"

void cs::PolygonColliderComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Polygon Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Use Mesh", &useMesh);
		ImGui::DragFloat("Friction", &friction, 0.01f);

		ImGui::TreePop();
	}
}
