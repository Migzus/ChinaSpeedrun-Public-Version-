#include "Rigidbody.h"

#include "ChinaEngine.h"

#include "GameObject.h"
#include "BSpline.h"
#include "Transform.h"
#include "PhysicsServer.h"
#include "Time.h"
#include "imgui.h"

#include "Color.h"
#include "Draw.h"

void cs::Rigidbody::CalculatePhysics(RigidbodyComponent& rigidBody, TransformComponent& transform)
{
	rigidBody.force += PhysicsServer::gravityDirection * rigidBody.mass * rigidBody.gravity;
	rigidBody.velocity += rigidBody.force / rigidBody.mass * Time::deltaTime;
	transform.position += rigidBody.velocity * PhysicsServer::airResistance * Time::deltaTime;
	rigidBody.force = Vector3(0.0f, 0.0f, 0.0f);
}

cs::RigidbodyComponent::RigidbodyComponent() :
	mass{ 1.0f }, gravity{ 9.81f }, velocity{ 0.0f }, force{ 0.0f }, startForce{ 0.0f }, endTime{ 0.0f }
{
	bodyType = BodyType::RIGID;
	//PhysicsBody::GetAllColliderComponents(this);
	//ChinaEngine::world.physicsServer->bodies.push_back(this);
}

void cs::RigidbodyComponent::ShootStartVelocity()
{
	velocity = startForce;
}

void cs::RigidbodyComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Disabled", &disabled);
		ImGui::DragFloat("Mass", &mass, 0.01f);
		ImGui::DragFloat("Gravity", &gravity, 0.01f);

		if (ImGui::DragFloat3("Start Force", &startForce[0], 0.01f) || ImGui::DragFloat("Time End", &endTime, 0.01f))
			UpdateTrajectory();
	
		ImGui::TreePop();
	}
}

void cs::RigidbodyComponent::AddForce(const Vector3 additionalForce)
{
	force += additionalForce;
}

// This function will be removed in the future, as it is spesific to visual simulations
void cs::RigidbodyComponent::UpdateTrajectory()
{
	if (gameObject->HasComponent<BSpline>())
	{
		TransformComponent& _transform{ gameObject->GetComponent<TransformComponent>() };
		BSpline& _bSpline{ gameObject->GetComponent<BSpline>() };
		
		_bSpline.points[0] = BSpline::Point3D(_transform.position, startForce * 0.5f);

		const Vector3 _endPosition{ Vector3(startForce.x * endTime, startForce.y * endTime - gravity * endTime * endTime * 0.5f, startForce.z * endTime) * PhysicsServer::airResistance };
		const Vector3 _endVelocity{ Vector3(startForce.x, startForce.y - gravity * endTime, startForce.z) * 0.5f * endTime * PhysicsServer::airResistance };
		_bSpline.points[1] = BSpline::Point3D(_transform.position + _endPosition, _endVelocity);

		_bSpline.UpdateMesh();
	}
}
