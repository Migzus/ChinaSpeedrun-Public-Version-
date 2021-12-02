#include "Mathf.h"

#include "PlaneCollider.h"

#include <random>
#include <ctime>

#include <glm/gtx/matrix_decompose.hpp>

void cs::Mathf::InitRand()
{
    srand((unsigned int)time(nullptr));
}

int cs::Mathf::Rand()
{
    return rand();
}

float cs::Mathf::RandRange(const float& min, const float& max)
{
    return min + (rand() / (float)RAND_MAX) * (max - min);
}

float cs::Mathf::Project(const Vector3& projVec, const Plane& plane)
{
    return std::abs(projVec.x * plane.normal.x + projVec.y * plane.normal.y + projVec.z * plane.normal.z + plane.length);
}

Vector3 cs::Mathf::Project(const Vector3& projVec, const Vector3& alignVec)
{
    return DotProduct(alignVec, projVec) / Magnitude(projVec) * projVec;
}

float cs::Mathf::Magnitude(const Vector3& vec)
{
    return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

Vector3 cs::Mathf::CrossProduct(const Vector3& vec1, const Vector3& vec2)
{
    return glm::cross(vec1, vec2);
}

float cs::Mathf::CrossProduct(const Vector2& vec1, const Vector2& vec2)
{
    return vec1.x * vec2.y - vec2.x * vec1.y;
}

float cs::Mathf::DotProduct(const Vector3& vec1, const Vector3& vec2)
{
    return glm::dot(vec1, vec2);
}

float cs::Mathf::DotProduct(const Vector2& vec1, const Vector2& vec2)
{
    return glm::dot(vec1, vec2);
}

float cs::Mathf::Max(const float& v1, const float& v2)
{
    return v1 * (v1 >= v2) + v2 * (v1 < v2);
}

float cs::Mathf::Min(const float& v1, const float& v2)
{
    return v1 * (v1 <= v2) + v2 * (v1 > v2);
}

bool cs::Mathf::Within(const float& a, const float& b, const float& c)
{
    return a <= b && b <= c;
}

bool cs::Mathf::IsVectorZero(const Vector2& vec)
{
    return vec.x == 0.0f || vec.y == 0.0f;
}

bool cs::Mathf::IsVectorZero(const Vector3& vec)
{
    return vec.x == 0.0f || vec.y == 0.0f || vec.z == 0.0f;
}

void cs::Mathf::DecomposeMatrix(const Matrix4x4& transform, Vector3& position, Vector3& rotation, Vector3& scale)
{
    Matrix4x4 _localMatrix{ transform };

    scale.x = glm::length(_localMatrix[0]);
    scale.y = glm::length(_localMatrix[1]);
    scale.z = glm::length(_localMatrix[2]);

    glm::normalize<3, float, glm::packed_highp>(_localMatrix[0]);
    glm::normalize<3, float, glm::packed_highp>(_localMatrix[1]);
    glm::normalize<3, float, glm::packed_highp>(_localMatrix[2]);

    rotation.x = atan2f(_localMatrix[1][2], _localMatrix[2][2]);
    rotation.y = atan2f(-_localMatrix[0][2], sqrtf(_localMatrix[1][2] * _localMatrix[1][2] + _localMatrix[2][2] * _localMatrix[2][2]));
    rotation.z = atan2f(_localMatrix[0][1], _localMatrix[0][0]);

    position = Vector3(_localMatrix[3]);
}
