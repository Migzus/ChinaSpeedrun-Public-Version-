#pragma once

#include "Resource.h"

#include <vector>

namespace cs
{
	class Shader : public Resource
	{
	public:
		Shader(std::vector<std::string> paths);

		enum class Type
		{
			VERTEX,
			FRAGMENT,
			GEOMETRY,
			COMPUTE
		};

		enum class DataType
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
			SAMPLER2D
		};

		// we need shader type, 
		void AssignShaderParam(Type shaderType, DataType dataType);

	private:
		//std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	};
}
