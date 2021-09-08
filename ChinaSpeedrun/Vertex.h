#pragma once

#include "vulkan/vulkan.h"
#include "engine/Mathf.h"

#include <array>

class Vertex
{
public:
	Vector3 position, color;
	Vector2 texCoord;

	static VkVertexInputBindingDescription GetBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions();
};

