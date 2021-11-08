#include "PhysicsBody.h"

#include "ChinaEngine.h"
#include "SceneManager.h"
#include "imgui.h"
#include "PhysicsServer.h"
#include "GameObject.h"
#include "Collider.h"
#include "SphereCollider.h"
#include "PolygonCollider.h"
#include "Rigidbody.h"
#include "StaticBody.h"
#include "Transform.h"

#include <type_traits>

void cs::PhysicsBody::GetAllColliderComponents(PhysicsBodyComponent* body)
{
	// We just have one reference for now...
	PhysicsObject* _pyObj{ new PhysicsObject };

	if (body->gameObject->HasComponent<SphereColliderComponent>())
		_pyObj->collider = &body->gameObject->GetComponent<SphereColliderComponent>();
	else if (body->gameObject->HasComponent<PolygonColliderComponent>())
		_pyObj->collider = &body->gameObject->GetComponent<PolygonColliderComponent>();
	else
	{
		delete _pyObj;
		return;
	}

	_pyObj->transform = &body->gameObject->GetComponent<TransformComponent>();

	body->colliders = _pyObj;
}

cs::CollisionInfo cs::PhysicsBody::CompareBodies(PhysicsBodyComponent* body, const PhysicsBodyComponent* otherBody)
{
	return body->colliders->collider->Intersect(body->colliders->transform, otherBody->colliders->collider, otherBody->colliders->transform);
}

cs::CollisionInfo cs::PhysicsBody::CompareBodies(RigidbodyComponent* body, const StaticBodyComponent* otherBody)
{
	return body->colliders->collider->Intersect(body->colliders->transform, otherBody->colliders->collider, otherBody->colliders->transform);
}

cs::CollisionInfo cs::PhysicsBody::CompareBodies(StaticBodyComponent* body, const RigidbodyComponent* otherBody)
{
	return body->colliders->collider->Intersect(body->colliders->transform, otherBody->colliders->collider, otherBody->colliders->transform);
}

cs::PhysicsBodyComponent::PhysicsBodyComponent() :
	bodyType{ BodyType::UNDEFINED }, disabled{ false }, colliders{ nullptr }
{
	//ChinaEngine::world.physicsServer->bodies.push_back(this);
}

void cs::PhysicsBodyComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Physics Body", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Disabled", &disabled);

		ImGui::TreePop();
	}
}

void cs::PhysicsBodyComponent::Init()
{
	PhysicsBody::GetAllColliderComponents(this);
	gameObject->GetScene()->GetPhysicsServer()->bodies.push_back(this);
}
