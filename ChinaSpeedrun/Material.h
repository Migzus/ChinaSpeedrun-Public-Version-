#pragma once

namespace cs
{
	class Material
	{
	public:
		class Shader* shader;

		Material(Shader* newShader);
	};
}
