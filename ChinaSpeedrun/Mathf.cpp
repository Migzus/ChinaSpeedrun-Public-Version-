#include "Mathf.h"

void cs::Mathf::Clamp(float& value, float min, float max)
{
    value =
        max * static_cast<float>(value > max) +
        min * static_cast<float>(value < min);
}

void cs::Mathf::LoopClamp(float& value, float min, float max)
{
    value =
        (value - max) * static_cast<float>(value > max) +
        (value - min + max) * static_cast<float>(value < min);
}
