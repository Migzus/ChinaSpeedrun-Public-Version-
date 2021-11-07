#include "StaticBody.h"

#include "ChinaEngine.h"
#include "SceneManager.h"
#include "Scene.h"
#include "PhysicsServer.h"

#include "imgui.h"

cs::StaticBodyComponent::StaticBodyComponent()
{
	bodyType = BodyType::STATIC;
	//SceneManager::GetCurrentScene()->physicsServer.push_back(this);
	//ChinaEngine::world.physicsServer->bodies.push_back(this);
}

void cs::StaticBodyComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Static Body", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Disabled", &disabled);

		ImGui::TreePop();
	}
}
