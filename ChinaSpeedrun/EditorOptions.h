#pragma once

#include "Mathf.h"

#include <string>
#include <iostream>

namespace cs
{
	namespace editor
	{
		class EditorOptions
		{
		public:
			static int windowWidth, windowHeight, windowXPos, windowYPos;

			static Vector3 cameraPosition;
			static Vector3 cameraRotation;

			static void LoadSettings();
			static void SaveSettings();

		private:
			enum class SettingsMode
			{
				NONE,
				WINDOW_SIZE,
				EDITOR_CAMERA
			};

			const static std::string optionsFilepath;

			static std::string WriteVector3(const Vector3& vector);
			static std::string GetDefaultSettings();
		};
	}
}
