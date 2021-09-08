#pragma once

/// <summary>
/// 
/// in the mesh renderer, we have to store information about the different descriptor offsets
/// because, multiple objects can have the same shader, the same material, the same mesh
/// Essentially, they can be duplicates, BUT they can have different orientations
/// 
/// </summary>

#include "Mesh.h"

// this will inherit from Component (because an object can have a mesh that renders on screen)
namespace cs
{
	class MeshRenderer
	{
	public:
		Mesh* mesh;
		struct UniformBufferObject* ubo;
		std::vector<VkDescriptorSet> descriptorSets; // make enough duplicates when inisializing this object

		MeshRenderer();
		MeshRenderer(Mesh* newMesh);

		const std::vector<VkDescriptorSet>& GetDescriptorSet() const;

	private:
	};
}
