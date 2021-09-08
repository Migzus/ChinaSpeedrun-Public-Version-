#pragma once

#include <vulkan/vulkan.h>
#include "Mathf.h"

#include <array>

namespace cs
{
	class Vertex
	{
	public:
		Vector3 position, color;
		Vector2 texCoord;

		static VkVertexInputBindingDescription GetBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions();
	};
}
