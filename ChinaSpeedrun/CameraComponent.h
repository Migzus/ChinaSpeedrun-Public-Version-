#pragma once

#include "Mathf.h"

namespace cs
{
	class CameraComponent
	{
	public:
		friend class Camera;

		float fov{ 50.0f }, nearPlane{ 0.01f }, farPlane{ 1000.0f };

	private:
		Matrix4x4 view, proj;
	};
}
