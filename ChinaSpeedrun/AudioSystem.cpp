#include "AudioSystem.h"
#include "AudioComponent.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AudioFile.h>

#include "Time.h"

cs::AudioBufferBlob::AudioBufferBlob() : index{ 0 }
{
	for (unsigned i = 0; i < max; i++) {
		buffer[i] = 0;
	}
}

cs::AudioSourceBlob::AudioSourceBlob() : index{ 0 }
{
	for (unsigned i = 0; i < max; i++) {
		source[i] = 0;
	}
}

#define OpenAL_ErrorCheck(message)\
{\
	ALenum error = alGetError();\
	if(error != AL_NO_ERROR)\
		std::cerr << "OpenAL Error: " << error << " function caled: " << #message << std::endl;\
}

#define alec(FUNCTION_CALL)\
FUNCTION_CALL;\
OpenAL_ErrorCheck(FUNCTION_CALL)

cs::AudioSystem::AudioSystem() {
	Init();
}

cs::AudioSystem::~AudioSystem()
{
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

void cs::AudioSystem::Update(AudioComponent& ac)
{
	if (ac.play)
	{
		if (ac.isPlaying)
		{
			Stop(ac.soundId);
		}

		ac.soundId = Play(ac.soundName);
		ac.duration = buffer.meta[soundMap[ac.soundName]].duration;
		ac.time = 0.f;
		ac.isPlaying = true;
		ac.play = false;
	}

	if (ac.stop)
	{
		Stop(ac.soundId);
		ac.soundId = 0;
		ac.time = 0.f;
		ac.isPlaying = false;
		ac.stop = false;
	}

	ac.time += Time::deltaTime * ac.isPlaying;
}

void cs::AudioSystem::Init()
{
	device = alcOpenDevice(nullptr);

	if (device) {
		context = alcCreateContext(device, nullptr);
		alcMakeContextCurrent(context);
	}
	else {
		return;
	}
	
	alec(alGenBuffers(buffer.max, buffer.buffer));
	alec(alGenSources(source.max, source.source));
	
	Load("../resources/sounds/koto.wav");
	Load("../resources/sounds/kazeoto.wav");
	Load("../resources/sounds/pon1.wav");
}

bool cs::AudioSystem::Load(std::string path) {

	if (buffer.index + 1 >= buffer.max)
	{
		std::cerr << "AudioSystem error: " << "Not enough buffers" << std::endl;
		return false;
	}

	std::vector<uint8_t> bufferData;

	AudioFile<float> file;
	if (!file.loadBuffer(path, bufferData))
	{
		std::cerr << "AudioFile error: " << "Couldn't read file" << std::endl;
		return false;
	}

	ALenum format;

	if (file.getNumChannelsAsRead() == 1)
	{
		if (file.getBitDepth() == 8)
		{
			format = AL_FORMAT_MONO8;
		}
		else
		{
			format = AL_FORMAT_MONO16;
		}
	}
	else if (file.getNumChannelsAsRead() == 2)
	{
		if (file.getBitDepth() == 8)
		{
			format = AL_FORMAT_STEREO8;
		}
		else
		{
			format = AL_FORMAT_STEREO16;
		}
	}

	buffer.meta[buffer.index].rate = file.getSampleRate();
	buffer.meta[buffer.index].depth = file.getBitDepth();
	buffer.meta[buffer.index].duration =
		static_cast<float>(bufferData.size()) /
		static_cast<float>(file.getBitDepth()) /
		static_cast<float>(file.getNumChannelsAsRead()) /
		static_cast<float>(file.getSampleRate()) *
		8.f;

	std::cout << bufferData.size() << ", " << file.getBitDepth() << ", " << file.getNumChannelsAsRead() << ", " << file.getSampleRate() << std::endl;

	alec(alBufferData(buffer[buffer.index], AL_FORMAT_STEREO16, bufferData.data(), static_cast<ALsizei>(bufferData.size()), file.getSampleRate()));

	auto pathToName = [](const std::string &path) {
		const size_t lastSlash = path.rfind('/');
		const size_t lastDot = path.rfind('.');
		return path.substr(lastSlash + 1, lastDot - lastSlash - 1);
	};

	soundMap.insert({ pathToName(path), buffer.index });

	buffer.index++;

	return true;
}

unsigned cs::AudioSystem::Play(std::string name)
{
	unsigned const _bi = soundMap[name];
	
	alSourcei(source[source.index], AL_BUFFER, buffer[_bi]);

	alSourcePlay(source[source.index]);

	const unsigned _sid{ source.index };

	source.index = (source.index + 1) % source.max;

	return _sid;
}

void cs::AudioSystem::Pause(unsigned sid)
{
	alSourcePause(source[sid]);
}

void cs::AudioSystem::Stop(unsigned sid)
{
	alSourceStop(source[sid]);
}

#undef alec
#undef OpenAL_ErrorCheck