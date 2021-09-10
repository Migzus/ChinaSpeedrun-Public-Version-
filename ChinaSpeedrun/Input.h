#pragma once

#include <map>
#include <string>
#include <vector>

struct GLFWwindow;

class Input
{
public:
	struct Key { bool held = false, pressed = false, released = false; };

	static void AddMapping(std::string name, int keycode);
	static void GlfwKeyfunCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods);
	static void FinishFrame();

	static bool GetActionHeld(std::string mappingName);
	static bool GetActionPressed(std::string mappingName);
	static bool GetActionReleased(std::string mappingName);

private:
	static std::map<std::string, Key*> mappings;
	static std::map<int, Key*> access;
	static std::vector<Key*> input;
};