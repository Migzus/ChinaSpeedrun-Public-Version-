#pragma once

#include "Component.h"

#include <string>

namespace cs
{
	class AudioComponent : Component
	{
	public:
		std::string soundName;
		unsigned soundId;
		bool play, stop, isPlaying, onListener;
		float time, duration;

		AudioComponent();

		virtual void ImGuiDrawComponent() override;
	};
}
