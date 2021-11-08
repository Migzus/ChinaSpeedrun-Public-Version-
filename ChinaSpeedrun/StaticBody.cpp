#include "StaticBody.h"

#include "PhysicsServer.h"

#include "imgui.h"

cs::StaticBodyComponent::StaticBodyComponent()
{
	bodyType = BodyType::STATIC;
	//PhysicsBody::GetAllColliderComponents(this);
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
