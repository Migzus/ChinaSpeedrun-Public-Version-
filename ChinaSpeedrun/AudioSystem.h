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

// Forward declare openal uint type
typedef unsigned int ALuint;

// Potential structs to group important data
struct AudioBufferBlob {
public:
	static constexpr ALuint max{ 10 };
	ALuint index;
	ALuint buffer[max];
	ALuint operator [](unsigned i) const { return buffer[i]; }
	ALuint& operator [](unsigned i) { return buffer[i]; }
	AudioBufferBlob();
private:
};

struct AudioSourceBlob {
public:
	static constexpr ALuint max{ 100 };
	ALuint index;
	ALuint source[max];
	ALuint operator [](ALuint i) const { return source[i]; }
	ALuint& operator [](ALuint i) { return source[i]; }
	AudioSourceBlob();
private:
};

class AudioComponent;
class TransformComponent;

class AudioSystem
{
public:
	AudioSystem();
	~AudioSystem();
	void Init();
	void Load(std::string path);
	ALuint Play(std::string name);
	void UpdatePlay(AudioComponent& ac);
	void UpdateLocation(AudioComponent& ac, const TransformComponent& tc);
private:
	std::map<std::string, unsigned> soundMap;
	AudioBufferBlob buffer;
	AudioSourceBlob source;
	class ALCdevice* device;
	class ALCcontext* context;
};