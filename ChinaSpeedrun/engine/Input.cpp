#include "Input.h"

std::map<std::string, Input::Key*> Input::mappings;
std::map<int, Input::Key*> Input::access;
std::vector<Input::Key*> Input::input;