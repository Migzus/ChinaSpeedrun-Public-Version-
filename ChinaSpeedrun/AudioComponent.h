#pragma once

#include <string>

class AudioComponent
{
public:
	std::string soundName;
	bool play, isPlaying, onListener;
	AudioComponent();
};

