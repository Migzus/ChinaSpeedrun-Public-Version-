#pragma once

#include <vulkan/vulkan.h>

#include "Mathf.h"

#include <vector>

namespace cs
{
	class Mesh
	{
	public:
		std::vector<class Material*> materials;
		VkDeviceSize vertexBufferOffset, indexBufferOffset;

		Mesh();
		Mesh(std::vector<class Vertex> vertexArray, std::vector<uint16_t> indexArray);

		static Mesh* CreateDefaultPlane(const Vector2 extent = Vector2(1.0f, 1.0f));
		static Mesh* CreateDefaultCube(const Vector3 extent = Vector3(1.0f, 1.0f, 1.0f));
		static Mesh* CreateDefaultIcoSphere(const uint8_t subdivisions = 2, const float radius = 1.0f);

		void SetMesh(std::vector<Vertex> vertexArray, std::vector<uint16_t> indexArray);
		std::vector<Vertex> const& GetVertices() const;
		std::vector<uint16_t> const& GetIndices() const;

		//bool operator==(const Mesh& otherMesh);
		//bool operator!=(const Mesh& otherMesh);

	private:
		std::vector<Vertex> vertices;
		std::vector<uint16_t> indices;
	};
}

