#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "Mathf.h"
#include "Resource.h"
#include "Vertex.h"

namespace cs
{
	class Mesh : public Resource
	{
	public:
		friend class VulkanEngineRenderer;
		friend class MeshRenderer;

		Mesh();
		Mesh(std::vector<class cs::Vertex> vertexArray, std::vector<uint32_t> indexArray);

		void Initialize() override;

		static Mesh* CreateDefaultPlane(const Vector2 extent = Vector2(1.0f, 1.0f));
		static Mesh* CreateDefaultCube(const Vector3 extent = Vector3(1.0f, 1.0f, 1.0f));
		static Mesh* CreateDefaultIcoSphere(const uint8_t subdivisions = 2, const float radius = 1.0f);

		void SetMesh(std::vector<Vertex> vertexArray, std::vector<uint32_t> indexArray);
		std::vector<Vertex> const& GetVertices() const;
		std::vector<uint32_t> const& GetIndices() const;

	private:
		VkBuffer vertexBufferRef, indexBufferRef;
		VkDeviceSize vertexBufferOffset, vertexSize, indexBufferOffset, indexSize;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};
}

