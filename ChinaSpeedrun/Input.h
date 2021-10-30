#pragma once

#include <map>
#include <string>
#include <vector>

#include "Mathf.h"

struct GLFWwindow;

class Input
{
public:
	struct Key { bool held{ false }, pressed{ false }, released{ false }; };

	static Vector2 mousePosition;
	static Vector2 mouseMovement;
	static Vector2 scrollOffset;

	static void AddMapping(std::string name, int keycode);
	static void RemoveMapping(std::string name, int keycode);
	static void GlfwKeyfunCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods);
	static void GlfwCursorPosCallback(GLFWwindow* window, double x, double y);
	static void GlfwScrollCallback(GLFWwindow* window, double offsetX, double offsetY);
	static void GlfwMouseButtonCallback(GLFWwindow* window, int mouseButton, int action, int mods);
	static void FinishFrame();

	static bool GetActionHeld(std::string mappingName);
	static bool GetActionPressed(std::string mappingName);
	static bool GetActionReleased(std::string mappingName);

	static bool GetMouseHeld(int mouseButton);
	static bool GetMousePressed(int mouseButton);
	static bool GetMouseReleased(int mouseButton);

private:
	static std::map<std::string, Key*> mappings;
	static std::map<int, Key*> access;
	static std::vector<Key*> input;

	static std::vector<int> heldMouseButtons;
	static std::vector<int> pressedMouseButtons;
	static std::vector<int> releasedMouseButtons;
};