#pragma once

#include <map>

#include "Resource.h"
#include "Variant.h"

namespace cs
{
	class Material : public Resource
	{
	public:
		enum class RenderMode
		{
			OPEQUE_,
			TRANSPARENT_ // because a TRANSPARENT macro is already defined by microsoft, we have to add the underscore
		} renderMode;
		
		enum class FillMode
		{
			FILL,
			LINE,
			POINT
		} fillMode;

		enum class CullMode
		{
			NONE,
			FRONT,
			BACK,
			BOTH
		} cullMode;

		std::map<std::string, void*> shaderParams;
		float lineWidth;
		class Shader* shader;

		Material();

		void Initialize() override;
	};
}
