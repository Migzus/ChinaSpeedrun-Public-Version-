#pragma once

#include "Component.h"

namespace cs
{
	class ScriptComponent : public Component
	{
	public:
		virtual void Init() override;
	};
}
