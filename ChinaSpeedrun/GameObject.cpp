#include "GameObject.h"

#include "Component.h"
#include "ChinaEngine.h"
#include "World.h"

#include "MeshRenderer.h"
#include "CameraComponent.h"
#include "Transform.h"
#include "RenderComponent.h"
#include "AudioComponent.h"
#include "SphereCollider.h"
#include "PolygonCollider.h"
#include "StaticBody.h"
#include "Rigidbody.h"

#include "Debug.h"

cs::GameObject::GameObject() :
	name { "Object" }, active{ true }
{}

void cs::GameObject::EditorDrawComponents()
{
	if (HasComponent<TransformComponent>())
		GetComponent<TransformComponent>().ImGuiDrawComponent();
	if (HasComponent<MeshRendererComponent>())
		GetComponent<MeshRendererComponent>().ImGuiDrawComponent();
	if (HasComponent<CameraComponent>())
		GetComponent<CameraComponent>().ImGuiDrawComponent();
	if (HasComponent<AudioComponent>())
		GetComponent<AudioComponent>().ImGuiDrawComponent();

	// Physics
	if (HasComponent<StaticBodyComponent>())
		GetComponent<StaticBodyComponent>().ImGuiDrawComponent();
	if (HasComponent<RigidbodyComponent>())
		GetComponent<RigidbodyComponent>().ImGuiDrawComponent();
	if (HasComponent<SphereColliderComponent>())
		GetComponent<SphereColliderComponent>().ImGuiDrawComponent();
	if (HasComponent<PolygonColliderComponent>())
		GetComponent<PolygonColliderComponent>().ImGuiDrawComponent();
}

void cs::GameObject::Init()
{

}

void cs::GameObject::Start()
{

}

void cs::GameObject::Update()
{

}

void cs::GameObject::FixedUpdate()
{

}

void cs::GameObject::ExitTree()
{

}

void cs::GameObject::QueueFree()
{

}

cs::GameObject::~GameObject()
{
	
}

void cs::GameObject::RemoveAllComponents()
{

}
