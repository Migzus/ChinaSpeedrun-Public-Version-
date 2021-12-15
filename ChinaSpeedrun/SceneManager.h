#pragma once

#include "Mathf.h"

#include "entt/entt.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace cs
{
	class Scene;
	class CameraBase;

	namespace editor
	{
		class ImGuiLayer;
	}

	/**
	* The Scene Manager handles the scene management. It manages all the scenes and keeps track of them.
	*/
	class SceneManager
	{
	private:
		enum class SceneAction
		{
			NONE,
			INIT,
			START,
			EXIT,
			FREE
		};

		struct SceneActionCombo
		{
			Scene* sceneRef;
			const SceneAction action;
		};

	public:
		friend editor::ImGuiLayer;

		static CameraBase* mainCamera;

		/*!
		* Create an empty object with no components at all.
		*/
		static class GameObject* InstanceEmptyObject(const char* name);
		/*!
		* Create a standard object with a transform component.
		* Here you may spesify what sort of orientation it may have.
		*/
		static GameObject* InstanceObject(const char* name, const Vector3 position = Vector3(0.0f), const Vector3 rotation = Vector3(0.0f), const Vector3 scale = Vector3(1.0f));

		/*!
		* A Vulkan based command that destroys all the descriptor pools for each render object.
		*/
		static void DestroyDescriptorPools();
		/*!
		* A Vulkan based command that creates all the descriptor pools for each render object.
		*/
		static void CreateDescriptorPools();
		/*!
		* Sets the current scene we want to focus on based in its index.
		*/
		static void SetCurrentFocusedScene(const uint32_t newCurrentScene);

		/*!
		* Solves a particular scene. You feed it a scene you want to solve, and then tell it what we want to do with the scene.
		* [SceneAction] is the action we want to take on the scene.
		*   -> NONE  : We do nothing with the scene
		*   -> INIT  : We call the scene's init function (used when initializing the scene)
		*	-> START : We call the scene's start function (used when focusing on a scene)
		*	-> EXIT  : We call the scene's exit function (used when unfocusing on a scene)
		*	-> FREE  : We free the scene from memory and destroy everything with it
		*/
		static void SolveScene(Scene* scene, const SceneAction action);
		/*!
		* Resolve resolves all the scenes we want to solve
		*/
		static void Resolve();
		/*!
		* We update the current scene
		*/
		static void Update();
		/*!
		* Checks if we have more than 1 scene in our scene tree
		*/
		static bool HasScenes();

		/*!
		* Sends input to the current scene (this is only used with input, to give input to the scripts)
		*/
		static void SendInput(int keycode, int scancode, int action, int mods);

		/*!
		* We get the current scene's registry
		*/
		static entt::registry& GetRegistry();

		/*!
		* We create a new scene
		* NOTE : This does not auto focus on the newly created scene nor does it automatically load it. Use "Load" and "SetCurrentFocusedScene" to focus on a new scene.
		*/
		static Scene* CreateScene(std::string name);
		/*!
		* Saves the current scene (It should already have a resource path, otherwise we ask to save somewhere)
		*/
		static void Save();
		/*!
		* Loads the scene in to memory
		*/
		static void Load(Scene* scene);
		/*!
		* Clears the scene from memory
		*/
		static void Unload(Scene* scene);
		/*!
		* Destroy all the active scenes
		* WARNING : This is very dangerous unless you know what you are doing.
		*/
		static void DestroyEverything();
		/*!
		* Gets the current scene
		*/
		static Scene* GetCurrentScene();

	private:
		static std::vector<SceneActionCombo> queueActionsForScenes;
		static std::vector<Scene*> activeScenes;
		static uint32_t currentScene;

		static void FreeScene(Scene* scene);
		static void DrawScenes();
	};
}
