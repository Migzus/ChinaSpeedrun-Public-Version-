#include "Debug.h"

#include "Texture.h"
#include "imgui.h"

std::deque<cs::Debug::DebugMessage> cs::Debug::messages;

void cs::Debug::ImGuiDrawMessages()
{
	//ImGui::Button("Clear");

	for (auto& msg : messages)
		ImGui::TextColored(Color::ColorToImVec4(msg.color), msg.message.c_str());
}

void cs::Debug::QueueMessage(Color color, std::string message)
{
	messages.push_back({ color, message });
	if (messages.size() > messageCapacity)
		messages.pop_front();
}
