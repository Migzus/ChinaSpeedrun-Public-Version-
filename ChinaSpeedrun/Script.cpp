#include "Script.h"

#include "imgui.h"
#include "lua.hpp"
#include "Debug.h"
#include "Input.h"

#include "Time.h"

cs::ScriptComponent::ScriptComponent() :
	luaState{ nullptr }, script{ nullptr }, luaOK{ false }
{}

void cs::ScriptComponent::Init()
{
	//GetGlobalTable();
}

void cs::ScriptComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Script", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto& variable : globalVariables)
		{
			switch (static_cast<LuaVariable::Type>(variable.type.index()))
			{
			case LuaVariable::Type::BOOL:
				ImGui::Checkbox(variable.name.c_str(), &std::get<bool>(variable.type));
				break;
			case LuaVariable::Type::INT:
				ImGui::DragInt(variable.name.c_str(), &std::get<int>(variable.type));
				break;
			case LuaVariable::Type::STRING:
			{
				char* _value{ std::get<std::string>(variable.type).data() };
				ImGui::InputText(variable.name.c_str(), _value, IM_ARRAYSIZE(_value));
				break;
			}
			case LuaVariable::Type::FLOAT:
				ImGui::DragFloat(variable.name.c_str(), &std::get<float>(variable.type));
				break;
			}
		}

		ImGui::TreePop();
	}
}

void cs::ScriptComponent::GetGlobalTable()
{
	if (!luaOK)
		return;

	lua_pushglobaltable(luaState);
	lua_pushnil(luaState);

	while (lua_next(luaState, -2) != 0)
	{
		if (std::string(lua_tostring(luaState, -2))[0] != '_') // ignore some pushed in lua stuff
		{
			// might do some fancy string parscing
			std::string _varName{ lua_tostring(luaState, -2) };

			// get the values
			if (lua_isboolean(luaState, -1))
				globalVariables.push_back({ _varName, static_cast<bool>(lua_toboolean(luaState, -1)) });
			else if (lua_isinteger(luaState, -1))
				globalVariables.push_back({ _varName, static_cast<int>(lua_tointeger(luaState, -1)) });
			else if (lua_isnumber(luaState, -1))
				globalVariables.push_back({ _varName, static_cast<float>(lua_tonumber(luaState, -1)) });
			else if (lua_isstring(luaState, -1))
				globalVariables.push_back({ _varName, std::string(lua_tostring(luaState, -1)) });
		}

		lua_pop(luaState, 1);
	}

	lua_pop(luaState, 1);
}

void cs::ScriptComponent::Start()
{
	if (!luaOK)
		return;

	if (lua_getglobal(luaState, "start") == LUA_TFUNCTION)
		lua_pcall(luaState, 0, 0, 0);
}

void cs::ScriptComponent::Update()
{
	if (!luaOK)
		return;

	if (lua_getglobal(luaState, "update") == LUA_TFUNCTION)
	{
		lua_pushnumber(luaState, static_cast<double>(Time::deltaTime));
		lua_pcall(luaState, 1, 0, 0);
	}
}

void cs::ScriptComponent::Input(char eventKey)
{
	if (!luaOK)
		return;

	if (lua_getglobal(luaState, "input") == LUA_TFUNCTION)
	{
		lua_pushinteger(luaState, eventKey);
		//lua_pushlightuserdata(luaState, &eventKey);
		lua_pcall(luaState, 1, 0, 0);
	}
}

void cs::ScriptComponent::Exit()
{
	if (!luaOK)
		return;

	if (lua_getglobal(luaState, "exit") == LUA_TFUNCTION)
	{
		lua_pcall(luaState, 0, 0, 0);
	}
}

void cs::ScriptComponent::SetScript(Script* newScript)
{
	if (newScript == nullptr)
		return;

	script = newScript;

	luaState = luaL_newstate();
	luaL_openlibs(luaState);

	luaOK = CheckLua("Could not read file", luaL_dofile(luaState, script->GetResourcePath().c_str()));
	
	GetGlobalTable();
}

cs::ScriptComponent::~ScriptComponent()
{
	lua_close(luaState);
}

bool cs::ScriptComponent::CheckLua(const char* errorMessage, int result)
{
	if (result != LUA_OK)
	{
		Debug::LogError("Lua Script Fail: ", errorMessage, " -> ", result);
		return false;
	}

	return true;
}

void cs::Script::Initialize()
{

}
