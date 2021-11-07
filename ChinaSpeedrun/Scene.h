#pragma once

#include "Resource.h"

#include "entt/entt.hpp"

#include <vector>
#include <string>

namespace cs
{
	class GameObject;
	class SceneManager;
	class VulkanEngineRenderer;

	// Scenes can also be treated as prefabs
	class Scene : public Resource
	{
	public:
		friend SceneManager;
		friend GameObject;
		friend VulkanEngineRenderer;

		Scene();

		// Initialize the components
		void Initialize() override;
		// Start the components
		void Start();
		// Update all of the components
		void Update();
		// Exits the scene and destroys it's contents.
		// Usually you don't call this yourself, for safety reasons.
		void Exit();

		// Adds a game object
		void AddGameObject(GameObject* newObject = nullptr);
		// Removes a selective object from the entire scene
		void RemoveGameObject();
		// Clears the scene's contents
		void ClearScene();
		// Queues this scene for deletion
		void QueueExit();

		void AddToRenderQueue(class RenderComponent* renderer);
		void RemoveFromRenderQueue(RenderComponent* renderer);

		uint32_t GetUBOOffset();

	private:
		bool ImGuiDrawGameObjects();
		void UpdateEditorComponents();
		void UpdateComponents();

		class PhysicsServer* physicsServer;
		class AudioSystem* audioSystem;

		entt::registry registry;
		std::string name;
		std::vector<GameObject*> gameObjects;
		std::vector<RenderComponent*> renderableObjects;
	};
}
