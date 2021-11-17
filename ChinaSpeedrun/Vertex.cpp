#include "Vertex.h"

VkVertexInputBindingDescription cs::Vertex::GetBindingDescription()
{
	VkVertexInputBindingDescription _bindingDescription{};
	_bindingDescription.binding = 0;
	_bindingDescription.stride = sizeof(Vertex);
	_bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return _bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> cs::Vertex::GetAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 3> _attributeDescriptions{};
	_attributeDescriptions[0].binding = 0;
	_attributeDescriptions[0].location = 0;
	_attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	_attributeDescriptions[0].offset = offsetof(Vertex, position);
	
	_attributeDescriptions[1].binding = 0;
	_attributeDescriptions[1].location = 1;
	_attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	_attributeDescriptions[1].offset = offsetof(Vertex, color);

	_attributeDescriptions[2].binding = 0;
	_attributeDescriptions[2].location = 2;
	_attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	_attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

	return _attributeDescriptions;
}

bool cs::Vertex::operator==(const Vertex& other) const
{
	return position == other.position && color == other.color && texCoord == other.texCoord;
}
