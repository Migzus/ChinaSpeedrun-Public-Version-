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
		static class VulkanEngineRenderer const& GetVulkanRenderer();
		static std::vector<char> ReadFile(const std::string& filename);

		static void Run();
		static std::vector<class MeshRenderer*> const& GetObjects();
		static std::vector<class Mesh*> const& GetMeshes();

	private:
		static VulkanEngineRenderer renderer;
		static std::vector<class Shader*> shaders;
		static std::vector<class Material*> materials;
		static std::vector<class Texture*> textures;
		static std::vector<Mesh*> meshes;
		static std::vector<MeshRenderer*> objects;

		static void EngineInit();
		static void MainLoop();
		static void EngineExit();
	};
}
