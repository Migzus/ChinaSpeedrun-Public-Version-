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
	struct DrawItem;

	/*!
	* The scenes are composed of game objects and holds the some of the simple systems.
	* Scenes can also be treated as prefabs
	* Scenes would have the ability to merge with each other
	*/
	class Scene : public Resource
	{
	public:
		friend SceneManager;
		friend GameObject;
		friend VulkanEngineRenderer;

		Scene();

		/*!
		* Initialize the components
		*/
		void Initialize() override;
		/*!
		* Start the components and scene
		*/
		void Start();
		/*!
		* Update all of the components in this scene
		*/
		void Update();
		/*!
		* Exits the scene and finishes the components
		* Usually you don't call this yourself, for safety reasons.
		*/
		void Exit();
		/*!
		* Instantly Frees this scene from memory
		* Called from the Scene Manager but made it so that you can have access.
		*/
		void Free();

		// Adds a game object
		void AddGameObject(GameObject* newObject = nullptr);
		/*!
		* Removes a selective object from the entire scene
		*/
		void RemoveGameObject();
		/*!
		* Clears the scene's contents
		*/
		void ClearScene();
		/*!
		* Queues this scene for deletion
		*/
		void QueueExit();

		/*!
		* Get this scene's Physics System
		*/
		class PhysicsServer* GetPhysicsServer() const;
		/*!
		* Get this scene's Audio System
		*/
		class AudioSystem* GetAudioSystem() const;

		/*!
		* Add to this scene's render queue (normally done automatically)
		*/
		void AddToRenderQueue(class RenderComponent* renderer);
		/*!
		* Removes Vulkan rendered related objects from this scene's render queue
		*/
		void RemoveFromRenderQueue(RenderComponent* renderer);

		uint32_t GetUBOOffset();

		/*!
		* Draw items related to this scene
		*/
		const std::vector<DrawItem*>& GetDrawItems() const;
		/*!
		* Push draw items to this scene
		*/
		void PushDebugItem(DrawItem* drawItem);
		
	private:
		void DestroyDescriptorPools();
		void CreateDescriptorPools();
		void Input(int keycode, int scancode, int action, int mods);

		bool ImGuiDrawGameObjects();
		void UpdateEditorComponents();
		void UpdateComponents();

		PhysicsServer* physicsServer;
		AudioSystem* audioSystem;

		entt::registry registry;
		std::string name;
		std::vector<GameObject*> gameObjects;
		std::vector<RenderComponent*> renderableObjects;
		std::vector<DrawItem*> drawItems;
	};
}
