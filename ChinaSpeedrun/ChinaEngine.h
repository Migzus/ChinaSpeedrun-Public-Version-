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
		static void InstanceObject(Mesh* mesh, class Material* material);
		static float AspectRatio();

	private:
		static VulkanEngineRenderer renderer;
		static std::vector<class Shader*> shaders;
		static std::vector<Material*> materials;
		static std::vector<class Texture*> textures;
		static std::vector<Mesh*> meshes;
		static std::vector<MeshRenderer*> objects;

		// resources would be textures, models, audio files and so on
		static std::vector<class Resource*> resources;
		// components would be something like a Transform, MeshRenderer and so on (things to effect resources)
		static std::vector<class Components*> components;

		static void EngineInit();
		static void InitInput();
		static void MainLoop();
		static void EngineExit();
	};
}
