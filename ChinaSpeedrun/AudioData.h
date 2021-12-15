#pragma once

#include <vector>

namespace cs {
	struct AudioMeta
	{
		float duration;
		unsigned rate;
		unsigned depth;
		unsigned channels;
		AudioMeta(float duration, unsigned rate, unsigned depth, unsigned channels);
	};

	class AudioData
	{
	public:
		AudioData(std::vector<uint8_t> buffer, AudioMeta meta);
		std::vector<float> pcm;
	private:
		std::vector<uint8_t> buffer;
		AudioMeta meta;
	};
}

