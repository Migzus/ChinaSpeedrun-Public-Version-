#pragma once

class AudioSystem
{
public:
	AudioSystem();
private:
	void Init();
	void PrintIfError();
	unsigned buffer;
	unsigned source;
	unsigned error;
	class ALCdevice* device;
	class ALCcontext* context;
	static constexpr unsigned bitDepth = 44100;
};