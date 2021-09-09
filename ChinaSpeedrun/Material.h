#pragma once

#include "Resource.h"

namespace cs
{
	class Material : public Resource
	{
	public:
		class Shader* shader;

		Material(Shader* newShader);
	};
}
