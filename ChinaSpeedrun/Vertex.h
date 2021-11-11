#pragma once

#include <vulkan/vulkan.h>
#include "Mathf.h"

#include <array>
#include <unordered_map>

namespace cs
{
	class Vertex
	{
	public:
		Vector3 position, color;
		Vector2 texCoord;
		//Vector3 normal;

		static VkVertexInputBindingDescription GetBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions();
		bool operator==(const Vertex& other) const;
	};
}

namespace std
{
	using namespace cs;

	template<>
	struct hash<Vertex>
	{
		size_t operator()(Vertex const& vertex) const
		{
			return ((hash<Vector3>()(vertex.position) ^ (hash<Vector3>()(vertex.color) << 1)) >> 1) ^ (hash<Vector2>()(vertex.texCoord) << 1);
		}
	};
}
