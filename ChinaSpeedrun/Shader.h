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
	class Draw;

	class Shader : public Resource
	{
	public:
		friend Draw;
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

		/*!
		* Here You finalize the vertex binding, and spesify what type of binding it is.
		* [VERTEX]
		*   Bind on a per vertex basis
		* [INSTANCE]
		*   Bind on a per instance basis (only if instance rendering is used)
		*/
		void AssignShaderVertexBinding(const InputRate& inputRate);
		/*!
		* Bind a descriptor to the shader. This is essentially a reference to the parameter you have made.
		* You can use the (binding = ???) tag to be spesific on where your variable is located in the shader program.
		*/
		void AssignShaderDescriptor(const std::string& descriptorName, const uint32_t& binding, const Type& shaderType, const Data& dataType);
		/*!
		* Add vertex attributes. Works in tangent with "AssignShaderVertexBinding"
		* Once you are done with assigning all of your attributes, you have to call
		* "AssignShaderVertexBinding" once.
		*/
		void AssignShaderVertexInputAttrib(const std::string& attrbuteName, const uint32_t& location, const Data& dataType);

		/*!
		* Gets the SPV code of this shader. It will be empty until it is solved in the renderer.
		*/
		const std::unordered_map<std::string, std::vector<char>>& GetSPVCode() const;

		/*!
		* Get the vulkan flag based on shader type
		*/
		static VkShaderStageFlagBits GetShaderStageFlag(Type typeName);
		/*!
		* Get the vulkan flag based on shader type name
		*/
		static VkShaderStageFlagBits GetShaderStageFlag(std::string typeName);

		~Shader();

	private:
		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout layout;

		std::unordered_map<std::string, std::vector<char>> spvCode;

		uint32_t currentVertexInputBindingDataSize, currentBinding;
	};
}
