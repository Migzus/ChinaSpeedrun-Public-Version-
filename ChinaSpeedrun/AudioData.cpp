#include "AudioData.h"

#include <utility>

cs::AudioMeta::AudioMeta(float duration, unsigned rate, unsigned depth, unsigned channels): duration(duration), rate(rate),
	depth(depth),
	channels(channels)
{
}

cs::AudioData::AudioData(std::vector<uint8_t> buffer, AudioMeta meta) : buffer{buffer}, meta{meta}
{
}
