#pragma once

// This is the object that we can instance in the scene
// We can attatch any component to this object

#include "World.h"
#include "ChinaEngine.h"
#include "Component.h"
#include "Debug.h"

#include <tuple>
#include <vector>
#include <string>
#include <set>

namespace cs
{
	namespace editor
	{
		class ImGuiLayer;
	}

	class GameObject
	{
	public:
		friend editor::ImGuiLayer;

		std::string name;
		bool active;
		entt::entity entity;
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
		// Gets the component at the specified index (This is the fastes method to get a component)
		template<class T>
		T& GetComponentAt(const uint8_t componentIndex);
		// Gets all of the components of this type
		template<class ...T>
		auto GetComponents();
		// Removes the selected
		template<class T>
		void RemoveComponent();
		// Removes all of the components stored
		void RemoveAllComponents();
		// Removes all the components of spsified type
		template<class T>
		void RemoveAllComponentsOfType();

		void GenerateOBBExtents();

		~GameObject();

	private:
		// Draw all the game object's components
		void EditorDrawComponents();
	};

	template<class ...T>
	inline bool GameObject::HasComponent()
	{
		return ChinaEngine::world.registry.any_of<T...>(entity);
	}

	template<class T>
	inline T& GameObject::AddComponent()
	{
		auto& _comp{ ChinaEngine::world.registry.emplace<T>(entity) };

		_comp.gameObject = this;
		GenerateOBBExtents();

		return _comp;
	}

	template<class T>
	inline T& GameObject::GetComponent()
	{
		return ChinaEngine::world.registry.get<T>(entity);
	}

	template<class T>
	inline T& GameObject::GetComponentAt(const uint8_t componentIndex)
	{
		return ChinaEngine::world.registry.get<componentIndex>(entity);
	}

	template<class ...T>
	auto GameObject::GetComponents()
	{
		return std::make_tuple(ChinaEngine::world.registry.get<T>(entity)...);
	}

	template<class T>
	inline void GameObject::RemoveComponent()
	{
		return ChinaEngine::world.registry.remove_if_exists<T>(entity);
	}

	template<class T>
	inline void GameObject::RemoveAllComponentsOfType()
	{

	}
}
