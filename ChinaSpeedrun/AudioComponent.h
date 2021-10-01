#pragma once

#include <string>

typedef unsigned int ALuint;

class AudioComponent
{
public:
	std::string soundName;
	ALuint sid;
	bool play, isPlaying, onListener;
	AudioComponent();
};

