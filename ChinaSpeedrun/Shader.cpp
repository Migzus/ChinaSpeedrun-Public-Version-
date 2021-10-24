#include "Shader.h"

#include "ChinaEngine.h"
#include "VulkanEngineRenderer.h"

cs::Shader::Shader(std::unordered_map<std::string, std::vector<char>> spv) : spvCode{ spv }
{
	Initialize();
}

void cs::Shader::Initialize()
{
	ChinaEngine::renderer.SolveShader(this, Solve::ADD);
}

void cs::Shader::AssignShaderDescriptor(std::string descriptorName, uint32_t binding, Type shaderType, Data dataType)
{
	VkDescriptorType _descriptorType{};

	switch (dataType)
	{
	case Data::SAMPLER2D:
		_descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		break;
	case Data::UNIFORM:
		_descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		break;
	}

	VkDescriptorSetLayoutBinding _layoutBinding{};
	_layoutBinding.binding = binding;
	_layoutBinding.descriptorCount = 1;
	_layoutBinding.descriptorType = _descriptorType;
	_layoutBinding.stageFlags = GetShaderStageFlag(shaderType);
	_layoutBinding.pImmutableSamplers = nullptr;

	descriptorBindings[descriptorName] = _layoutBinding;
}

void cs::Shader::AssignShaderVertexInputAttrib(std::string attrbuteName, uint32_t location, Data dataType, uint32_t offset)
{
	VkFormat _format{ VK_FORMAT_UNDEFINED };

	switch (dataType)
	{
	case Data::BOOL:
	case Data::INT:
		_format = VK_FORMAT_R32_SINT;
		break;
	case Data::FLOAT:
		_format = VK_FORMAT_R32_SFLOAT;
		break;
	case Data::VEC2:
		_format = VK_FORMAT_R32G32_SFLOAT;
		break;
	case Data::VEC3:
		_format = VK_FORMAT_R32G32B32_SFLOAT;
		break;
	case Data::VEC4:
		_format = VK_FORMAT_R32G32B32A32_SFLOAT;
		break;
	case Data::MAT2:
		break;
	case Data::MAT3:
		break;
	case Data::MAT4:
		break;
	default:
		_format = VK_FORMAT_UNDEFINED;
		break;
	}

	VkVertexInputAttributeDescription _attributeDescription{};
	_attributeDescription.binding = 0;
	_attributeDescription.location = location;
	_attributeDescription.format = _format;
	_attributeDescription.offset = offset;

	vertexAttributes[attrbuteName] = _attributeDescription;
}

const std::unordered_map<std::string, std::vector<char>>& cs::Shader::GetSPVCode() const
{
	return spvCode;
}

VkShaderStageFlagBits cs::Shader::GetShaderStageFlag(Type typeName)
{
	switch (typeName)
	{
	case Type::VERTEX:
		return VK_SHADER_STAGE_VERTEX_BIT;
	case Type::FRAGMENT:
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	case Type::GEOMETRY:
		return VK_SHADER_STAGE_GEOMETRY_BIT;
	case Type::COMPUTE:
		return VK_SHADER_STAGE_COMPUTE_BIT;
	case Type::ANY:
		return VK_SHADER_STAGE_ALL_GRAPHICS;
	}

	return VK_SHADER_STAGE_ALL;
}

VkShaderStageFlagBits cs::Shader::GetShaderStageFlag(std::string typeName)
{
	if (typeName == "vert")
		return VK_SHADER_STAGE_VERTEX_BIT;
	else if (typeName == "frag")
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	else if (typeName == "geom")
		return VK_SHADER_STAGE_GEOMETRY_BIT;
	else if (typeName == "comp")
		return VK_SHADER_STAGE_COMPUTE_BIT;
	else if (typeName.empty())
		return VK_SHADER_STAGE_ALL_GRAPHICS;

	return VK_SHADER_STAGE_ALL;
}
