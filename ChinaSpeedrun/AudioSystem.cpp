#include "AudioSystem.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AudioFile.h>

AudioBufferBlob::AudioBufferBlob() : index{ 0 }
{
	for (unsigned i = 0; i < max; i++) {
		buffer[i] = 0;
	}
}

AudioSourceBlob::AudioSourceBlob() : index{ 0 }
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

AudioSystem::AudioSystem() {
	Init();
}

AudioSystem::~AudioSystem()
{
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

void AudioSystem::Init()
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
}

void AudioSystem::Load(std::string path) {

	if (buffer.index + 1 >= buffer.max) {
		std::cerr << "AudioSystem error: " << "Not enough buffers" << std::endl;
		return;
	}

	AudioFile<float> file;
	file.load(path);

	ALenum format;

	if (file.isMono()) {
		if (file.getBitDepth() == 8) {
			format = AL_FORMAT_MONO8;
		}
		else {
			format = AL_FORMAT_MONO16;
		}
	}
	else {
		if (file.getBitDepth() == 8) {
			format = AL_FORMAT_STEREO8;
		}
		else {
			format = AL_FORMAT_STEREO16;
		}
	}

	std::vector<uint8_t> bufferData;

	file.convertPCMToBuffer(bufferData);

	alec(alBufferData(buffer[buffer.index], AL_FORMAT_STEREO16, bufferData.data(), bufferData.size(), file.getSampleRate()));

	auto pathToName = [](const std::string &path) {
		const unsigned lastSlash = path.rfind('/');
		const unsigned lastDot = path.rfind('.');
		return path.substr(lastSlash + 1, lastDot - lastSlash - 1);
	};

	soundMap.insert({ pathToName(path), buffer.index });

	buffer.index++;
}

void AudioSystem::Play(std::string name)
{
	unsigned const _bi = soundMap[name];
	
	alSourcei(source[source.index], AL_BUFFER, buffer[_bi]);

	alSourcePlay(source[source.index]);

	source.index = (source.index + 1) % source.max;
}

#undef alec
#undef OpenAL_ErrorCheck