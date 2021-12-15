#include "Timer.h"

//#include "Signal.h"
#include "GameObject.h"
#include "imgui.h"
#include "Time.h"

cs::Timer::Timer() :
	startOnAwake{ false }, timeout{ 0.0f }, isActive{ false }, currentTime{ 0.0f }
{}

void cs::Timer::Init()
{
	//gameObject->signals.Connect("timeout", this, &Timer::Stop);

	if (startOnAwake)
		Start();
}

void cs::Timer::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Timer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Start On Awake", &startOnAwake);
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
		//gameObject->signals.EmitSignal("timeout");
		Stop();
	}
}
