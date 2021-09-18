#include "AudioSystem.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AudioFile.h>

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

	{
		AudioFile<float> file;
		file.load("../resources/sounds/koto.wav");
		/*std::cout << file.isMono() << " " << file.isStereo() << " " << file.getSampleRate() << " " << file.getBitDepth() << " " << file.getLengthInSeconds() << " " << file.getNumChannels() << " " << file.getNumSamplesPerChannel() << std::endl;
		std::cout << sizeof(file) << "\n" << sizeof(file.samples) << "\n" << sizeof(file.samples[0]) << "\n" << sizeof(file.samples[0][0]) << "\n";
		std::cout << file.samples.size() << " " << file.samples[0].size() << "\n";*/
		
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

		std::cout << bufferData.size() << std::endl;

		alGenBuffers(1, &buffer);
		PrintIfError();
		alec(alBufferData(buffer, AL_FORMAT_STEREO16, bufferData.data(), bufferData.size(), file.getSampleRate()));
		PrintIfError();
	}
	
	alec(alGenSources(1, &source));
	alec(alSourcei(source, AL_BUFFER, buffer));
	alec(alSourcePlay(source));
}

void AudioSystem::PrintIfError()
{
	if ((error = alGetError()) != AL_NO_ERROR) {
		std::cerr << "Error: " << error << std::endl;
	}
}

#undef alec
#undef OpenAL_ErrorCheck
