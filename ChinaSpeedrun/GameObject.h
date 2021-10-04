#pragma once

// This is the object that we can instance in the scene
// We can attatch any component to this object

#include "entt/entt.hpp"

#include "ChinaEngine.h"

#include <vector>
#include <string>

namespace cs
{
	class GameObject
	{
	public:
		std::string name;
		bool active;
		entt::entity entity;
		// TODO : Make this hashable (it's faster and we can make it readable)
		std::vector<std::string> tags;

		GameObject();

		// Called at the start of the program
		virtual void Init();
		// Called when the object is instanced
		virtual void Start();
		// Called every frame
		virtual void Update();
		// Called every physics frame
		virtual void FixedUpdate();
		// Called when the object is destroyed
		virtual void ExitTree();

		// Queues this object for deletion
		void QueueFree();
		
		// Add the spesified component
		template<class T>
		void AddComponent();
		// Gets a component of this type
		template<class T>
		T* GetComponent(const uint8_t componentSkipCount = 0);
		// Gets the component at the specified index (This is the fastes method to get a component)
		template<class T>
		T* GetComponentAt(const uint8_t componentIndex);
		// Gets all of the components of this type
		template<class T>
		std::vector<T*> GetComponents();
		// Removes the selected
		template<class T>
		void RemoveComponent(const T* component);
		// Removes all of the components stored
		void RemoveAllComponents();
		// Removes all the components of spsified type
		template<class T>
		void RemoveAllComponentsOfType();

		~GameObject();

	protected:
		virtual void ComponentInit();
	};

	template<class T>
	inline void GameObject::AddComponent()
	{
		//ChinaEngine::world.regestry.emplace<T>(entity);
	}

	template<class T>
	inline T* GameObject::GetComponent(const uint8_t componentSkipCount)
	{

	}

	template<class T>
	inline T* GameObject::GetComponentAt(const uint8_t componentIndex)
	{

	}

	template<class T>
	inline std::vector<T*> GameObject::GetComponents()
	{

	}

	template<class T>
	inline void GameObject::RemoveComponent(const T* component)
	{

	}

	template<class T>
	inline void GameObject::RemoveAllComponentsOfType()
	{

	}
}
