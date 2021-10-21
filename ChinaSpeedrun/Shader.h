#pragma once

#include "Resource.h"

#include <vulkan/vulkan.h>
#include <map>

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

		void Initialize() override;

		void AssignShaderDescriptor(std::string descriptorName, uint32_t binding, Type shaderType, Data dataType);
		void AssignShaderVertexInputAttrib(std::string attrbuteName, uint32_t location, Data dataType, uint32_t offset);

	private:
		VkRenderPass renderPass;
		VkPipelineLayout layout;
		VkPipeline pipeline;
		//std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	};
}
