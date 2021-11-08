#include "AudioComponent.h"

#include "imgui.h"

cs::AudioComponent::AudioComponent() : soundId{ 0 }, play{ false }, stop{ false }, isPlaying{ false }, onListener{ false }, time{ 0 }, duration{ 0 }
{}

void cs::AudioComponent::Init()
{

}

void cs::AudioComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Audio", ImGuiTreeNodeFlags_DefaultOpen))
	{
		char buf[128];
		strncpy_s(buf, soundName.c_str(), sizeof(buf) - 1);
		ImGui::InputText("Sound Name", &buf[0], sizeof(buf));
		soundName = buf;

		if (ImGui::Button("Play"))
			play = true;

		ImGui::SameLine();

		if (ImGui::Button("Stop"))
			stop = true;

		ImGui::SameLine();
		ImGui::ProgressBar(time / duration);

		ImGui::TreePop();
	}
}
