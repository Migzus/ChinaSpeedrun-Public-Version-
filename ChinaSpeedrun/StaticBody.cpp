#include "StaticBody.h"

#include "ChinaEngine.h"
#include "World.h"
#include "PhysicsServer.h"

#include "imgui.h"

cs::StaticBodyComponent::StaticBodyComponent()
{
	bodyType = BodyType::STATIC;
	ChinaEngine::world.physicsServer->bodies.push_back(this);
}

void cs::StaticBodyComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Static Body", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Disabled", &disabled);

		ImGui::TreePop();
	}
}
