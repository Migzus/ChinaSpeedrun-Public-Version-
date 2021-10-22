#pragma once

#include "Resource.h"

#include <vulkan/vulkan.h>
#include <map>
#include <vector>

namespace cs
{
	class Shader : public Resource
	{
	public:
		enum class Type
		{
			VERTEX,
			FRAGMENT,
			GEOMETRY,
			COMPUTE
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

		std::map<std::string, VkDescriptorSetLayoutBinding> descriptorBindings;
		std::map<std::string, VkVertexInputAttributeDescription> vertexAttributes;

		Shader(std::map<std::string, std::vector<char>> spv);

		void Initialize() override;

		void AssignShaderDescriptor(std::string descriptorName, uint32_t binding, Type shaderType, Data dataType);
		void AssignShaderVertexInputAttrib(std::string attrbuteName, uint32_t location, Data dataType, uint32_t offset);

		const std::map<std::string, std::vector<char>>& GetSPVCode() const;

	private:
		VkRenderPass renderPass;
		VkPipelineLayout layout;
		VkPipeline pipeline;

		std::map<std::string, std::vector<char>> spvCode;
		//std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	};
}
