#include "CameraBase.h"

cs::CameraBase::CameraBase() :
	fov{ 45.0f }, nearPlane{ 0.01f }, farPlane{ 1000.0f }, leftPlane{ 200.0f },
	rightPlane{ 200.0f }, topPlane{ 200.0f }, bottomPlane{ 200.0f }, projection{ Projection::PERSPECTIVE },
	view{ Matrix4x4(1.0f) }, proj{ Matrix4x4(1.0f) }
{}

void cs::CameraBase::SetExtents(const float& width, const float& height)
{
	bottomPlane = topPlane = height * 0.5f;
	bottomPlane = -bottomPlane;
	leftPlane = rightPlane = width * 0.5f;
	leftPlane = -leftPlane;
}
