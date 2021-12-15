#include "GameObject.h"

#include "Component.h"

#include "Scene.h"
#include "SceneManager.h"

cs::GameObject::GameObject() :
	name{ "Object" }, active{ true }, entity{ SceneManager::GetRegistry().create() }, parent{ nullptr }, scene{ nullptr }
{
	//signals.Connect("enter_tree", this, &GameObject::EnterTree);
	//signals.Connect("exit_tree", this, &GameObject::ExitTree);
}

cs::GameObject::GameObject(Scene* newScene) :
	name{ "Object" }, active{ true }, entity{ newScene->registry.create() }, parent{ nullptr }, scene{ newScene }
{
	//signals.Connect("enter_tree", this, &GameObject::EnterTree);
	//signals.Connect("exit_tree", this, &GameObject::ExitTree);
}

void cs::GameObject::EnterTree()
{
	//signals.EmitSignal("enter_tree");
}

void cs::GameObject::EditorDrawComponents()
{
	for (auto* component : components)
		component->ImGuiDrawComponent();
}

void cs::GameObject::ExitTree()
{
	RemoveAllComponents();

	scene->registry.destroy(entity);
}

void cs::GameObject::QueueFree()
{
	// this function remains empty because we have yet to use it
}

cs::GameObject::~GameObject()
{
	//signals.Disconnect("enter_tree", this, &GameObject::EnterTree);
	//signals.Disconnect("exit_tree", this, &GameObject::ExitTree);
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

cs::GameObject* cs::GameObject::GetParent()
{
	return parent;
}

bool cs::GameObject::HasParent()
{
	return parent != nullptr;
}

void cs::GameObject::AddChild(GameObject* childObject)
{
	childObject->parent = this;
	children.push_back(childObject);
}

const std::vector<cs::GameObject*>& cs::GameObject::GetChildren()
{
	return children;
}
