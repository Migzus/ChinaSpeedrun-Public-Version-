#include "PhysicsServer.h"

#include "imgui.h"
#include "Time.h"

void cs::RigidBody::CalculatePhysics(RigidBodyComponent& rigidBody, TransformComponent& transform)
{
	rigidBody.velocity += rigidBody.gravityDirection * (rigidBody.gravity / rigidBody.mass) * Time::deltaTime;

	transform.position += rigidBody.velocity * Time::deltaTime;
}

void cs::PhysicsBody::GetAllColliderComponents(PhysicsBodyComponent& body)
{
	//std::vector<Collider&> _components{ body.gameObject->GetComponents<Collider>() };
}

void cs::PhysicsBody::CompareBodies(PhysicsBodyComponent& body, const PhysicsBodyComponent& otherBody)
{

}

void cs::PhysicsBodyComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Static Body", ImGuiTreeNodeFlags_DefaultOpen))
	{


		ImGui::TreePop();
	}
}

void cs::SphereColliderComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Sphere Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{


		ImGui::TreePop();
	}
}

void cs::RigidBodyComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat("Mass", &mass, 0.01f);
		ImGui::DragFloat("Gravity", &gravity, 0.01f);

		ImGui::TreePop();
	}
}

void cs::StaticBodyComponent::ImGuiDrawComponent()
{

}
