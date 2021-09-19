#pragma once

/*
	Currently, when the AudioFile library reads an audio file it
	1) converts it from RAW buffer data into readable PCM data
	2) converts it back back to RAW buffer data
	The first step can be optimized away with further modifications to the AudioFile library
	(and will most likely be done later)
*/

#include <string>
#include <map>

// Potential structs to group important data
struct AudioBufferBlob {
public:
	static constexpr unsigned max{ 10 };
	unsigned index;
	unsigned buffer[max];
	unsigned operator [](unsigned i) const { return buffer[i]; }
	unsigned& operator [](unsigned i) { return buffer[i]; }
	AudioBufferBlob();
private:
};

struct AudioSourceBlob {
public:
	static constexpr unsigned max{ 100 };
	unsigned index;
	unsigned source[max];
	unsigned operator [](unsigned i) const { return source[i]; }
	unsigned& operator [](unsigned i) { return source[i]; }
	AudioSourceBlob();
private:
};

class AudioSystem
{
public:
	AudioSystem();
	~AudioSystem();
	void Init();
	void Load(std::string path);
	void Play(std::string name);

private:
	std::map<std::string, unsigned> soundMap;
	AudioBufferBlob buffer;
	AudioSourceBlob source;
	class ALCdevice* device;
	class ALCcontext* context;
};