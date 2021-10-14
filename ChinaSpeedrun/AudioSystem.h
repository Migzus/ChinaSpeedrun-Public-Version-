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

class ALCdevice;
class ALCcontext;

namespace cs {
	struct AudioMeta
	{
		float duration;
		unsigned rate;
		unsigned depth;
	};

	struct AudioBufferBlob {
		static constexpr unsigned max{ 10 };
		unsigned index;
		unsigned buffer[max];
		AudioMeta meta[max];
		unsigned operator [](const unsigned i) const { return buffer[i]; }
		unsigned& operator [](const unsigned i) { return buffer[i]; }
		AudioBufferBlob();
	};

	struct AudioSourceBlob {
		static constexpr unsigned max{ 100 };
		unsigned index;
		unsigned source[max];
		unsigned operator [](unsigned i) const { return source[i]; }
		unsigned& operator [](unsigned i) { return source[i]; }
		AudioSourceBlob();
	};

	class AudioSystem
	{
	public:
		AudioSystem();
		~AudioSystem();
		
		void Update(class AudioComponent& ac);

	private:
		void Init();
		bool Load(std::string path);
		unsigned Play(std::string name);
		void Pause(unsigned sid);
		void Stop(unsigned sid);

		std::map<std::string, unsigned> soundMap;
		AudioBufferBlob buffer;
		AudioSourceBlob source;
		ALCdevice* device;
		ALCcontext* context;
	};
}