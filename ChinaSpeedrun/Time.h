#pragma once

#include <chrono>

namespace cs
{
	class Time
	{
	public:
		static float time;
		static float deltaTime;
		static float fixedDeltaTime; // for physics (currently unused)

		static void CycleInit();
		static void CycleStart();
		static void CycleEnd();

	private:
		static std::chrono::steady_clock::time_point initTime;
		static std::chrono::steady_clock::time_point startFrameTime;
	};
}
