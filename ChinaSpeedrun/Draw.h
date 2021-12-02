#pragma once

#include "Mathf.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace cs
{
	class Mesh;
	class Scene;

	struct DrawItem
	{
		Mesh* mesh;
		std::vector<VkDescriptorSet*> descriptorSetsRef;

		DrawItem();

		void UpdateMesh();
		void SetDescriptorRefs(std::vector<VkDescriptorSet>& sets);
	};

	class Draw
	{
	public:
		friend class VulkanEngineRenderer;

		enum class DrawMode
		{
			SOLID,
			DASHED
		};

		static void Setup();

		static void Update();

		static void DebugGrid();
		static auto MakeLine(const Vector3& startPos, const Vector3& endPos);
		static cs::DrawItem* Line(const std::vector<Vector3>& points, std::vector<class Color> colors, const DrawMode& drawMode, Scene* sceneToAttatchTo = nullptr);
		static cs::DrawItem* Line(const std::vector<Vector3>& points, std::vector<Color> colors, const DrawMode& drawMode, std::vector<VkDescriptorSet>& descriptorSetsRef, Scene* sceneToAttatchTo = nullptr);
		static void MeshLine(Mesh* mesh, Scene* sceneToAttatchTo = nullptr);
		static void Rectangle(const Vector3& extents, const Vector3& position, Scene* sceneToAttatchTo = nullptr);
		static void Circle(const float& radius, const Vector3& position, const Color& color, const uint32_t& resolution = 16, Scene* sceneToAttatchTo = nullptr);
		static void Image(const class Texture* texture, const Vector3& position, const Vector3& size = Vector3(1.0f), Scene* sceneToAttatchTo = nullptr);
		static void VulkanDraw(VkCommandBuffer& commandBuffer, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer);
		static void CreateDescriptorSets();

		static class Material* material;
	private:
		static bool updateVertexIndexBuffers;
		static std::vector<DrawItem*> debugItems;
		static VkDescriptorPool descriptorPool;
		static std::vector<VkDescriptorSet> descriptorSets;
		static struct UniformBufferObject ubo;
	};
}
