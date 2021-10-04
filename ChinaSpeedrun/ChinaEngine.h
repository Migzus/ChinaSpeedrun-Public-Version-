#pragma once

#include "ChinaCoreHeader.h"

#include "Mathf.h"

namespace cs
{
	struct UniformBufferObject
	{
		Matrix4x4 model, view, proj;
	};

	class ChinaEngine
	{
	public:
		static class VulkanEngineRenderer renderer;

		static void Run();
		static std::vector<class MeshRenderer*> const& GetObjects();
		static MeshRenderer* InstanceObject(class Mesh* mesh, class Material* material, const Vector3 position);
		static float AspectRatio();

	private:
		static std::vector<MeshRenderer*> objects;

		static void EngineInit();
		static void ImGuiStyleInit();
		static void InitInput();
		static void ImGuiDraw();
		static void MainLoop();
		static void EngineExit();
	};
}
