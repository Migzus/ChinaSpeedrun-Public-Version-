#pragma once

#include "Mathf.h"

namespace cs
{
	namespace editor
	{
		class EditorCamera
		{
		public:
			constexpr static float fov{ 50.0f };
			constexpr static float nearPlane{ 0.01f };
			constexpr static float farPlane{ 1000.0f };
			constexpr static float orientationSpeed{ 1.0f };
			constexpr static float movementsSpeed{ 1.0f };

			Vector3 position, rotation;

			void Update();

		private:
			Matrix4x4 view, proj;
		};
	}
}
