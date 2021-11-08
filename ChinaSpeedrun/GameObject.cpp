#include "GameObject.h"

#include "Component.h"
#include "ChinaEngine.h"
#include "Mathf.h"

#include "MeshRenderer.h"
#include "CameraComponent.h"
#include "Transform.h"
#include "RenderComponent.h"
#include "AudioComponent.h"
#include "SphereCollider.h"
#include "PolygonCollider.h"
#include "StaticBody.h"
#include "Rigidbody.h"

#include "SceneManager.h"

cs::GameObject::GameObject() :
	name{ "Object" }, active{ true }, entity{ SceneManager::GetRegistry().create() }, parent{ nullptr }
{}

void cs::GameObject::EditorDrawComponents()
{
	for (auto* component : components)
		component->ImGuiDrawComponent();

	/*if (HasComponent<TransformComponent>())
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
		GetComponent<PolygonColliderComponent>().ImGuiDrawComponent();*/
}

void cs::GameObject::ExitTree()
{
	RemoveAllComponents();

	scene->registry.destroy(entity);
}

void cs::GameObject::QueueFree()
{

}

cs::GameObject::~GameObject()
{
	
}

std::vector<cs::Component*> cs::GameObject::GetAllComponents()
{
	return components;
}

void cs::GameObject::RemoveAllComponents()
{
	scene->registry.destroy(entity);
	entity = scene->registry.create();
}

cs::Scene* cs::GameObject::GetScene() const
{
	return scene;
}
