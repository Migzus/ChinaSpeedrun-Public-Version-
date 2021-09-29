#include "Time.h"

float cs::Time::time{ 0.0f };
float cs::Time::deltaTime{ 0.0f };
float cs::Time::fixedDeltaTime{ 0.0f };
std::chrono::steady_clock::time_point cs::Time::initTime;
std::chrono::steady_clock::time_point cs::Time::startFrameTime;

void cs::Time::CycleInit()
{
	initTime = std::chrono::high_resolution_clock::now();
}

void cs::Time::CycleStart()
{
	startFrameTime = std::chrono::high_resolution_clock::now();
	time = std::chrono::duration<float, std::chrono::seconds::period>(startFrameTime - initTime).count();
}

void cs::Time::CycleEnd()
{
	auto _endFrameTime{ std::chrono::high_resolution_clock::now() };
	deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(_endFrameTime - startFrameTime).count();
}
