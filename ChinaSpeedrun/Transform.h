#pragma once

#include "Mathf.h"

namespace cs
{
	class TransformComponent
	{
	public:
		friend class Transform;

		Vector3 position{ Vector3(0.0f, 0.0f, 0.0f) }, rotation{ Vector3(0.0f, 0.0f, 0.0f) }, scale{ Vector3(1.0f, 1.0f, 1.0f) };
	protected:
		Matrix4x4 matrix{ Matrix4x4(1.0f) };
	};

	class Transform
	{
	public:
		static void CalculateMatrix(TransformComponent& transform);
	};
}
