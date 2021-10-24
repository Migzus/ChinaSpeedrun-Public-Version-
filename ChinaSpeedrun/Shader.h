#pragma once

#include "Resource.h"

#include <vulkan/vulkan.h>
#include <unordered_map>
#include <vector>

namespace cs
{
	class Shader : public Resource
	{
	public:
		friend class VulkanEngineRenderer;
		friend class ResourceManager;

		enum class Type
		{
			VERTEX,
			FRAGMENT,
			GEOMETRY,
			COMPUTE,
			ANY
		};

		enum class Data
		{
			FLOAT,
			BOOL,
			INT,
			VEC2,
			VEC3,
			VEC4,
			MAT2,
			MAT3,
			MAT4,
			SAMPLER2D,
			UNIFORM
		};

		std::unordered_map<std::string, VkDescriptorSetLayoutBinding> descriptorBindings;
		std::unordered_map<std::string, VkVertexInputAttributeDescription> vertexAttributes;

		Shader(std::unordered_map<std::string, std::vector<char>> spv);

		void Initialize() override;

		void AssignShaderDescriptor(std::string descriptorName, uint32_t binding, Type shaderType, Data dataType);
		void AssignShaderVertexInputAttrib(std::string attrbuteName, uint32_t location, Data dataType, uint32_t offset);

		const std::unordered_map<std::string, std::vector<char>>& GetSPVCode() const;

		static VkShaderStageFlagBits GetShaderStageFlag(Type typeName);
		static VkShaderStageFlagBits GetShaderStageFlag(std::string typeName);

	private:
		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout layout;

		std::unordered_map<std::string, std::vector<char>> spvCode;
	};
}
