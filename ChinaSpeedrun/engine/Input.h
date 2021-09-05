#pragma once

#include <map>
#include <string>
#include <vector>

struct GLFWwindow;

class Input
{
public:
	struct Key { bool held = false, pressed = false, released = false; };

	static void AddMapping(std::string name, int keycode) {

		if (mappings.find(name) == mappings.end()) {

			auto accessIter = access.find(keycode);

			if (accessIter == access.end()) {
				Key* keyref = new Key;
				input.push_back(keyref);
				access.insert({ keycode, keyref });
				mappings.insert({ name, keyref });
			}
			else {
				mappings.insert({ name, accessIter->second });
			}
		}
	}

	static void GlfwKeyfunCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods) {
		if (access.find(keycode) != access.end()) {
			access.at(keycode)->held |= (action == 1);
			access.at(keycode)->pressed |= (action == 1);
			access.at(keycode)->released |= (action == 0);
		}
	}

	static void Update() {
		for (auto& k : input) {
			k->held |= k->pressed;
			k->pressed = false;
			k->held &= !k->released;
			k->released = false;
		}
	}

	static bool GetActionHeld(std::string mappingName) {
		return mappings.at(mappingName)->held;
	}

	static bool GetActionPressed(std::string mappingName) {
		return mappings.at(mappingName)->pressed;
	}

	static bool GetActionReleased(std::string mappingName) {
		return mappings.at(mappingName)->released;
	}

private:
	static std::map<std::string, Key*> mappings;
	static std::map<int, Key*> access;
	static std::vector<Key*> input;
};