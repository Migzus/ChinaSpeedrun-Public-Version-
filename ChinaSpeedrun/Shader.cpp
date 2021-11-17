#include "Shader.h"

#include "ChinaEngine.h"
#include "VulkanEngineRenderer.h"

cs::Shader::Shader(std::unordered_map<std::string, std::vector<char>> spv) :
	descriptorSetLayout{ nullptr }, layout{ nullptr }, spvCode{ spv }, currentVertexInputBindingDataSize{ 0 }, currentBinding{ 0 }
{
	Initialize();
}

void cs::Shader::Initialize()
{
	ChinaEngine::renderer.SolveShader(this, Solve::ADD);
}

void cs::Shader::AssignShaderVertexBinding(const InputRate& inputRate)
{
	VkVertexInputRate _inputRate{ VK_VERTEX_INPUT_RATE_VERTEX };

	switch (inputRate)
	{
	case InputRate::VERTEX:
		_inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		break;
	case InputRate::INSTANCE:
		_inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		break;
	}

	VkVertexInputBindingDescription _vertexInstanceBinding{};
	_vertexInstanceBinding.binding = currentBinding;
	_vertexInstanceBinding.inputRate = _inputRate;
	_vertexInstanceBinding.stride = currentVertexInputBindingDataSize;

	currentBinding++;
	currentVertexInputBindingDataSize = 0;

	vertexInputDescription.push_back(_vertexInstanceBinding);
}

void cs::Shader::AssignShaderDescriptor(const std::string& descriptorName, const uint32_t& binding, const Type& shaderType, const Data& dataType)
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

void cs::Shader::AssignShaderVertexInputAttrib(const std::string& attrbuteName, const uint32_t& location, const Data& dataType)
{
	VkFormat _format{ VK_FORMAT_UNDEFINED };
	uint32_t _offset{ 0 };

	switch (dataType)
	{
	case Data::BOOL:
	case Data::INT:
		_format = VK_FORMAT_R32_SINT;
		_offset = sizeof(signed int);
		break;
	case Data::FLOAT:
		_format = VK_FORMAT_R32_SFLOAT;
		_offset = sizeof(float);
		break;
	case Data::VEC2:
		_format = VK_FORMAT_R32G32_SFLOAT;
		_offset = sizeof(float) * 2;
		break;
	case Data::VEC3:
		_format = VK_FORMAT_R32G32B32_SFLOAT;
		_offset = sizeof(float) * 3;
		break;
	case Data::VEC4:
		_format = VK_FORMAT_R32G32B32A32_SFLOAT;
		_offset = sizeof(float) * 4;
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
	_attributeDescription.binding = currentBinding;
	_attributeDescription.location = location;
	_attributeDescription.format = _format;
	_attributeDescription.offset = currentVertexInputBindingDataSize;

	currentVertexInputBindingDataSize += _offset;

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

cs::Shader::~Shader()
{
	ChinaEngine::renderer.SolveShader(this, Solve::REMOVE, true);
}
