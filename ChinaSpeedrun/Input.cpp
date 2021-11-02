#include "Input.h"

#include "imgui_impl_glfw.h"

#include "Debug.h"

#include <GLFW/glfw3.h>

std::map<std::string, Input::Key*> Input::mappings;
std::map<int, Input::Key*> Input::access;
std::vector<Input::Key*> Input::input;
Vector2 Input::mousePosition;
Vector2 Input::scrollOffset;
Vector2 Input::mouseMovement;

std::vector<int> Input::heldMouseButtons;
std::vector<int> Input::pressedMouseButtons;
std::vector<int> Input::releasedMouseButtons;

void Input::AddMapping(std::string name, int keycode)
{
	if (mappings.find(name) == mappings.end())
	{
		auto _accessIter{ access.find(keycode) };

		if (_accessIter == access.end())
		{
			Key* _keyref{ new Key };
			input.push_back(_keyref);
			access.insert({ keycode, _keyref });
			mappings.insert({ name, _keyref });
		}
		else
			mappings.insert({ name, _accessIter->second });
	}
	else
		cs::Debug::LogInfo("This mapping name already exists.");
}

void Input::RemoveMapping(std::string name, int keycode)
{
	return; // still doesn't actually work... (maybe because of some similar keys)

	auto _it{ mappings.find(name) };

	if (_it != mappings.end())
	{
		Key* _keyref{ _it->second };

		mappings.erase(name);
		access.erase(keycode);
		input.erase(std::find(input.begin(), input.end(), _keyref));

		delete _keyref;
	}
	else
		cs::Debug::LogInfo("Cannot delete a non existing key-bind.");
}

void Input::GlfwKeyfunCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods)
{
	ImGui_ImplGlfw_KeyCallback(window, keycode, scancode, action, mods);

	if (access.find(keycode) == access.end())
		return;

	access.at(keycode)->held |= (action == 1);
	access.at(keycode)->pressed |= (action == 1);
	access.at(keycode)->released |= (action == 0);
}

void Input::GlfwCursorPosCallback(GLFWwindow* window, double x, double y)
{
	Vector2 _prevMousePosition{ mousePosition };
	mousePosition = { x, y };
	mouseMovement = mousePosition - _prevMousePosition;
}

void Input::GlfwScrollCallback(GLFWwindow* window, double offsetX, double offsetY)
{
	ImGui_ImplGlfw_ScrollCallback(window, offsetX, offsetY);

	scrollOffset = { offsetX, offsetY };
}

void Input::GlfwMouseButtonCallback(GLFWwindow* window, int mouseButton, int action, int mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(window, mouseButton, action, mods);

	switch (action)
	{
	case GLFW_PRESS:
		pressedMouseButtons.push_back(mouseButton);
		heldMouseButtons.push_back(mouseButton);
		break;
	case GLFW_RELEASE:
	{
		auto _it{ std::find(heldMouseButtons.begin(), heldMouseButtons.end(), mouseButton) };
		if (_it != heldMouseButtons.end())
			heldMouseButtons.erase(_it);
		
		releasedMouseButtons.push_back(mouseButton);
		break;
	}
	default:
		cs::Debug::LogInfo("A non registered action from the mouse was sent.");
		break;
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

	scrollOffset = mouseMovement = { 0.0, 0.0 };

	pressedMouseButtons.clear();
	releasedMouseButtons.clear();
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

bool Input::GetMouseHeld(int mouseButton)
{
	return std::find(heldMouseButtons.begin(), heldMouseButtons.end(), mouseButton) != heldMouseButtons.end();
}

bool Input::GetMousePressed(int mouseButton)
{
	return std::find(pressedMouseButtons.begin(), pressedMouseButtons.end(), mouseButton) != pressedMouseButtons.end();
}

bool Input::GetMouseReleased(int mouseButton)
{
	return std::find(releasedMouseButtons.begin(), releasedMouseButtons.end(), mouseButton) != releasedMouseButtons.end();
}
