#include "Mathf.h"

void cs::Mathf::Clamp(float& value, float min, float max)
{
    value =
        max * (value > max) +
        min * (value < min);
}

void cs::Mathf::LoopClamp(float& value, float min, float max)
{
    value =
        (value - max) * (value > max) +
        (value - min + max) * (value < min);
}
