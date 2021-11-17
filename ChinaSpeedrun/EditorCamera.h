#pragma once

#include "CameraBase.h"

namespace cs
{
	class Camera;

	namespace editor
	{
		class EditorCamera : public CameraBase
		{
		public:
			friend Camera;

			class EngineEditor* editorRoot;
			Vector3 position, rotation;
			float movementsSpeed, rotationSpeed;

			EditorCamera(EngineEditor* root);

			void Update();

		private:
			void RotateCamera();
			void Movement();
			void ScrollAdjustmentSpeed();
			void SelectTest();
		};
	}
}
