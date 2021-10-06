#pragma once

#include <string>

namespace cs
{
	class AudioComponent
	{
	public:
		std::string soundName;
		unsigned soundId;
		bool play, stop, isPlaying, onListener;
		float time, duration;

		AudioComponent();
	};
}
