#include "Mathf.h"

#include "PolygonCollider.h"

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

float cs::Mathf::Project(const Vector3 projVec, const Plane plane)
{
    return std::abs(projVec.x * plane.normal.x + projVec.y * plane.normal.y + projVec.z * plane.normal.z + plane.length);
}

Vector3 cs::Mathf::Project(const Vector3 projVec, const Vector3 alignVec)
{
    return DotProduct(alignVec, projVec) / Magnitude(projVec) * projVec;
}

float cs::Mathf::Magnitude(const Vector3 vec)
{
    return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

Vector3 cs::Mathf::CrossProduct(const Vector3 vec1, const Vector3 vec2)
{
    return glm::cross(vec1, vec2);
}

float cs::Mathf::DotProduct(const Vector3 vec1, const Vector3 vec2)
{
    return glm::dot(vec1, vec2);
}
