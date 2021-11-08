#include "Rigidbody.h"

#include "ChinaEngine.h"

#include "Transform.h"
#include "PhysicsServer.h"
#include "Time.h"
#include "imgui.h"

void cs::Rigidbody::CalculatePhysics(RigidbodyComponent& rigidBody, TransformComponent& transform)
{
	rigidBody.force += PhysicsServer::gravityDirection * rigidBody.mass * rigidBody.gravity;
	rigidBody.velocity += rigidBody.force / rigidBody.mass * PhysicsServer::airResistance * Time::deltaTime;
	transform.position += rigidBody.velocity * Time::deltaTime;
	rigidBody.force = Vector3(0.0f, 0.0f, 0.0f);
}

cs::RigidbodyComponent::RigidbodyComponent() :
	mass{ 0.1f }, gravity{ 9.81f }, velocity{ 0.0f, 0.0f, 0.0f }, force{ 0.0f, 0.0f, 0.0f }
{
	bodyType = BodyType::RIGID;
	//PhysicsBody::GetAllColliderComponents(this);
	//ChinaEngine::world.physicsServer->bodies.push_back(this);
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

void cs::RigidbodyComponent::AddForce(const Vector3 additionalForce)
{
	force += additionalForce;
}
