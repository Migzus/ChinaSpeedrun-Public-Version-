#pragma once

#include "Component.h"
#include "CameraBase.h"

namespace cs
{
	class CameraComponent : public Component, public CameraBase
	{
	public:
		friend class Camera;

		//static CameraComponent* currentActiveCamera;

		enum class Projection
		{
			ORTHOGRAPHIC,
			PERSPECTIVE
		} projection;

		virtual void Init() override;
		virtual void ImGuiDrawComponent() override;
	};
}
