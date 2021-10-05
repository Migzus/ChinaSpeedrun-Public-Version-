#pragma once

#include "Mathf.h"

namespace cs {
	class CameraComponent
	{
	public:
		Matrix4x4 view, proj;
		CameraComponent();
	};
}
