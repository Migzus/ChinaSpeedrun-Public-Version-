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

		/*!
		* Updates the mesh this draw item relates to (e.g. updates the vertices position...)
		*/
		void UpdateMesh();
		void SetDescriptorRefs(std::vector<VkDescriptorSet>& sets);
	};

	/**
	* A static class that is able to converse with the vulkan engine and slap gizmos and other
	* editor elements in the scene
	*/
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

		/*!
		* Update Draw elements / items
		*/
		static void Update();

		/*!
		* Make the world grid (helpful in situations where you want to see things relative to the world origin)
		*/
		static void DebugGrid();
		/*!
		* Draws a line from and to the spesified points
		*/
		static auto MakeLine(const Vector3& startPos, const Vector3& endPos);
		/*!
		* Draws a line through all af the spesified points. Orientation is relative to the world
		*/
		static cs::DrawItem* Line(const std::vector<Vector3>& points, std::vector<class Color> colors, const DrawMode& drawMode, Scene* sceneToAttatchTo = nullptr);
		/*!
		* Draws a line through all af the spesified points. Orientation is relative to an object (because of the parameter 'descriptorSetsRef')
		*/
		static cs::DrawItem* Line(const std::vector<Vector3>& points, std::vector<Color> colors, const DrawMode& drawMode, std::vector<VkDescriptorSet>& descriptorSetsRef, Scene* sceneToAttatchTo = nullptr);
		/*!
		* Mesh Line draws a mesh, but all of the triangles are lines
		* Is this useful? Maybe. Depends on the situation.
		*/
		static void MeshLine(Mesh* mesh, Scene* sceneToAttatchTo = nullptr);
		static void Rectangle(const Vector3& extents, const Vector3& position, Scene* sceneToAttatchTo = nullptr);
		static void Circle(const float& radius, const Vector3& position, const Color& color, const uint32_t& resolution = 16, Scene* sceneToAttatchTo = nullptr);
		static void Image(const class Texture* texture, const Vector3& position, const Vector3& size = Vector3(1.0f), Scene* sceneToAttatchTo = nullptr);
		/*!
		* The Vulkan Draw commands that are only meant to draw the draw elements
		*/
		static void VulkanDraw(VkCommandBuffer& commandBuffer, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer);
		/*!
		* Create descriptor sets for all base elements.
		* In other words, we create the descriptor sets for all the objects that we want only to be relative to world
		*/
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
