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

		static class GameObject* InstanceEmptyObject(const char* name);
		static GameObject* InstanceObject(const char* name, const Vector3 position = Vector3(0.0f), const Vector3 rotation = Vector3(0.0f), const Vector3 scale = Vector3(1.0f));

		static void DestroyDescriptorPools();
		static void CreateDescriptorPools();
		static void SetCurrentFocusedScene(const uint32_t newCurrentScene);

		static void SolveScene(Scene* scene, const SceneAction action);
		static void Resolve();
		static void Update();
		static bool HasScenes();

		static entt::registry& GetRegistry();

		static Scene* CreateScene(std::string name);
		static void Save(); // saves the current scene (it should already have a resource path, otherwise we ask to save somewhere)
		static void Load(Scene* scene); // load the scene in from memory
		static void Unload(Scene* scene); // unload it from the scene
		static void UnloadEverything();
		static Scene* GetCurrentScene();

	private:
		static std::vector<SceneActionCombo> queueActionsForScenes;
		static std::vector<Scene*> activeScenes;
		static uint32_t currentScene;

		static void FreeScene(Scene* scene);
		static void DrawScenes();
	};
}
