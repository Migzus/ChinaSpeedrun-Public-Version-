#pragma once

#include "Resource.h"

#include <vulkan/vulkan.h>
#include <unordered_map>
#include <vector>

namespace cs
{
	class VulkanEngineRenderer;
	class ResourceManager;
	class MeshRendererComponent;
	class BulletManagerComponent;

	class Shader : public Resource
	{
	public:
		friend VulkanEngineRenderer;
		friend ResourceManager;
		friend MeshRendererComponent;
		friend BulletManagerComponent;

		enum class InputRate
		{
			VERTEX,
			INSTANCE
		};

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

		std::vector<VkVertexInputBindingDescription> vertexInputDescription;
		std::unordered_map<std::string, VkDescriptorSetLayoutBinding> descriptorBindings;
		std::unordered_map<std::string, VkVertexInputAttributeDescription> vertexAttributes;

		Shader(std::unordered_map<std::string, std::vector<char>> spv);

		void Initialize() override;

		void AssignShaderVertexBinding(const InputRate& inputRate);
		void AssignShaderDescriptor(const std::string& descriptorName, const uint32_t& binding, const Type& shaderType, const Data& dataType);
		void AssignShaderVertexInputAttrib(const std::string& attrbuteName, const uint32_t& location, const Data& dataType);

		const std::unordered_map<std::string, std::vector<char>>& GetSPVCode() const;

		static VkShaderStageFlagBits GetShaderStageFlag(Type typeName);
		static VkShaderStageFlagBits GetShaderStageFlag(std::string typeName);

		~Shader();

	private:
		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout layout;

		std::unordered_map<std::string, std::vector<char>> spvCode;

		uint32_t currentVertexInputBindingDataSize, currentBinding;
	};
}
