#pragma once

#include "Mathf.h"

namespace cs
{
	class CameraBase
	{
	public:
		friend class Camera;

		float fov, nearPlane, farPlane, leftPlane, rightPlane, topPlane, bottomPlane;

		enum class Projection
		{
			ORTHOGRAPHIC,
			PERSPECTIVE
		} projection;

		CameraBase();

		void SetExtents(const float& width, const float& height);

	protected:
		Matrix4x4 view, proj;
	};
}
