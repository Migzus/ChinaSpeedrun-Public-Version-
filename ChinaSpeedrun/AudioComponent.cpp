#include "AudioComponent.h"

cs::AudioComponent::AudioComponent() : soundId{ 0 }, play{ false }, stop{ false }, isPlaying{ false }, onListener{ false }, time{ 0 }, duration{ 0 }
{
}
