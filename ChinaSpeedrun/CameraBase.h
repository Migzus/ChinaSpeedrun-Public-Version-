#pragma once

#include "Mathf.h"

namespace cs
{
	class CameraBase
	{
	public:
		friend class Camera;

		float fov{ 45.0f }, nearPlane{ 0.01f }, farPlane{ 1000.0f };

	protected:
		Matrix4x4 view{ Matrix4x4(1.0f) }, proj{ Matrix4x4(1.0f) };
	};
}
