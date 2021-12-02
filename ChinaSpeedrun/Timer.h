#pragma once

#include "Component.h"

namespace cs
{
	class Timer : public Component
	{
	public:
		float timeout;

		Timer();

		virtual void Init() override;
		virtual void ImGuiDrawComponent() override;

		void Start();
		void Start(const float& timeout);
		void Stop();
		void Pause(const bool& status);
		void StepTimer();

	private:
		bool isActive;
		float currentTime;
	};
}
