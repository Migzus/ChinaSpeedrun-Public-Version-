#pragma once

#include "Component.h"

#include <string>

namespace cs
{
	class AudioComponent : public Component
	{
	public:
		std::string soundName;
		unsigned soundId;
		bool play, stop, isPlaying, onListener;
		float time, duration;

		AudioComponent();

		virtual void Init() override;
		virtual void ImGuiDrawComponent() override;
	};
}
