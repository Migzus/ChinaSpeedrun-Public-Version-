#include "Shader.h"

//#include "vulkan/vulkan.h"
//#include "ChinaEngine.h" // will move all vulkan logic in a separate class, current work around

cs::Shader::Shader(std::vector<std::string> paths)
{
	for (size_t i{ 0 }; i < paths.size(); i++)
	{
		//auto _shaderCode{ cs::ChinaEngine::ReadFile(paths[i]) };



		/*VkShaderModule _typeShaderModule{};//{ CreateShaderModule(_shaderCode) };

		VkShaderStageFlagBits _stageFlag{};

		if (paths[i].find("vert") != -1)
			_stageFlag = VK_SHADER_STAGE_VERTEX_BIT;
		else if (paths[i].find("frag") != -1)
			_stageFlag = VK_SHADER_STAGE_FRAGMENT_BIT;
		else if (paths[i].find("----") != -1)
			_stageFlag = VK_SHADER_STAGE_GEOMETRY_BIT;
		else if (paths[i].find("----") != -1)
			_stageFlag = VK_SHADER_STAGE_COMPUTE_BIT;
		else
			throw std::runtime_error("[ERROR] :\tCannot find a siutable shader type for -> " + paths[i]);
			*/
		
	}
}
