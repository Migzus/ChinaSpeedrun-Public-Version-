#pragma once

// This is the object that we can instance in the scene
// We can attatch any component to this object

#include "Scene.h"
#include "ChinaEngine.h"
#include "Component.h"
#include "Mathf.h"
#include "Debug.h"

#include <tuple>
#include <vector>
#include <string>
#include <set>

namespace cs
{
	class SceneManager;

	namespace editor
	{
		class ImGuiLayer;
	}

	class GameObject
	{
	public:
		friend editor::ImGuiLayer;
		friend SceneManager;
		friend Scene;

		std::string name;
		bool active;
		OBB obb;
		// TODO : Make this hashable (it's faster and we can make it readable)
		std::set<std::string> tags;

		GameObject();

		// Called when the object is destroyed
		virtual void ExitTree();

		// Queues this object for deletion
		void QueueFree();
		
		// Checks to see if we have at least one of the component type
		template<class ...T>
		bool HasComponent();
		// Add the spesified component
		template<class T>
		T& AddComponent();
		// Gets a component of this type
		template<class T>
		T& GetComponent();
		// Gets a constant component reference of this type
		template<class T>
		const T& GetComponentConst() const;
		// Gets all of the components of this type
		template<class ...T>
		auto GetComponents();
		// Gets all of the components
		std::vector<Component*> GetAllComponents();
		// Removes the selected
		template<class T>
		void RemoveComponent();
		// Removes all of the components stored
		void RemoveAllComponents();

		Scene* GetScene() const;

		~GameObject();

	private:
		entt::entity entity;
		Scene* scene;

		GameObject* parent;
		std::vector<GameObject*> children;
		std::vector<Component*> components;

		// Draw all the game object's components
		void EditorDrawComponents();
	};

	template<class ...T>
	inline bool GameObject::HasComponent()
	{
		return scene->registry.any_of<T...>(entity);
	}

	template<class T>
	inline T& GameObject::AddComponent()
	{
		auto& _comp{ scene->registry.emplace<T>(entity) };

		components.push_back(&_comp);
		_comp.gameObject = this;
		_comp.Init();

		return _comp;
	}

	template<class T>
	inline T& GameObject::GetComponent()
	{
		return scene->registry.get<T>(entity);
		return ChinaEngine::world.registry.get<T>(entity);
	}

	template<class T>
	inline const T& GameObject::GetComponentConst() const
	{
		return static_cast<const T&>(ChinaEngine::world.registry.get<T>(entity));
	}
	
	template<class ...T>
	auto GameObject::GetComponents()
	{
		return std::make_tuple(scene->registry.get<T>(entity)...);
	}

	template<class T>
	inline void GameObject::RemoveComponent()
	{
		scene->registry.remove_if_exists<T>(entity);
	}
}
