#pragma once

#include "ChinaCoreHeader.h"

#include "Mathf.h" // Soon this include will be removed (Once InstanceObject is moved to World)

namespace cs
{
	class ChinaEngine
	{
	public:
		static class World world;
		static class VulkanEngineRenderer renderer;

		static void Run();
		static std::vector<class GameObject*> const& GetObjects();
		static GameObject* InstanceObject(const char* name, const Vector3 position = Vector3(1.0f, 1.0f, 1.0f), const Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f), const Vector3 scale = Vector3(1.0f, 1.0f, 1.0f));
		static float AspectRatio();

	private:
		static std::vector<GameObject*> objects;

		static void EngineInit();
		static void ImGuiStyleInit();
		static void InitInput();
		static void ImGuiDraw();
		static void MainLoop();
		static void EngineExit();
	};
}
