#include "Input.h"

#include "imgui_impl_glfw.h"

std::map<std::string, Input::Key*> Input::mappings;
std::map<int, Input::Key*> Input::access;
std::vector<Input::Key*> Input::input;

void Input::AddMapping(std::string name, int keycode)
{
	if (mappings.find(name) == mappings.end()) {

		auto _accessIter{ access.find(keycode) };

		if (_accessIter == access.end()) {
			Key* _keyref{ new Key };
			input.push_back(_keyref);
			access.insert({ keycode, _keyref });
			mappings.insert({ name, _keyref });
		}
		else {
			mappings.insert({ name, _accessIter->second });
		}
	}
}

void Input::GlfwKeyfunCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods)
{
	ImGui_ImplGlfw_KeyCallback(window, keycode, scancode, action, mods);

	if (access.find(keycode) != access.end())
	{
		access.at(keycode)->held |= (action == 1);
		access.at(keycode)->pressed |= (action == 1);
		access.at(keycode)->released |= (action == 0);
	}
}

void Input::FinishFrame()
{
	for (auto& k : input)
	{
		k->held |= k->pressed;
		k->pressed = false;
		k->held &= !k->released;
		k->released = false;
	}
}

bool Input::GetActionHeld(std::string mappingName)
{
	return mappings.at(mappingName)->held;
}

bool Input::GetActionPressed(std::string mappingName)
{
	return mappings.at(mappingName)->pressed;
}

bool Input::GetActionReleased(std::string mappingName)
{
	return mappings.at(mappingName)->released;
}
