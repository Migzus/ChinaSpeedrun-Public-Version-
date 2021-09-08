#pragma once

#include <vector>
#include <string>

namespace cs
{
	class Shader
	{
	public:
		Shader(std::vector<std::string> paths);
	private:
		//std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	};
}
