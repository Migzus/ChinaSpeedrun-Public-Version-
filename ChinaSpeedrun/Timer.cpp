#include "Timer.h"

#include "Signal.h"
#include "GameObject.h"
#include "imgui.h"
#include "Time.h"

cs::Timer::Timer() :
	timeout{ 0.0f }, isActive{ false }, currentTime{ 0.0f }
{}

void cs::Timer::Init()
{
	// hook up a signal to the gameObject
}

void cs::Timer::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat("Timeout", &timeout);

		ImGui::TreePop();
	}
}

void cs::Timer::Start()
{
	isActive = true;
}

void cs::Timer::Start(const float& timeout)
{
	this->timeout = timeout;
	Start();
}

void cs::Timer::Stop()
{
	currentTime = timeout;
	isActive = false;
}

void cs::Timer::Pause(const bool& status)
{
	isActive = status;
}

void cs::Timer::StepTimer()
{
	if (!isActive)
		return;

	currentTime -= Time::deltaTime;

	if (currentTime < 0.0f)
	{
		//gameObject->signal.EmitSignal("Timeout");
		Stop();
	}
}
