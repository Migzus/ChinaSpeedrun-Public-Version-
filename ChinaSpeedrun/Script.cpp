#include "Script.h"

#include "imgui.h"
#include "lua.hpp"
#include "Debug.h"
#include "Input.h"

#include "Time.h"
#include "Editor.h"

#include "ResourceManager.h"
#include "Transform.h"
#include "CameraComponent.h"
#include "MeshRenderer.h"
#include "SpriteRenderer.h"
#include "BSpline.h"
#include "StaticBody.h"
#include "Rigidbody.h"

cs::ScriptComponent::ScriptComponent() :
	luaState{ nullptr }, script{ nullptr }, luaOK{ false }
{}

void cs::ScriptComponent::Init()
{

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

	/*
	lua_pushinteger(luaState, 262); // right
	lua_setfield(luaState, -2, "KEY_RIGHT_ARROW");
	lua_pushinteger(luaState, 263); // left
	lua_setfield(luaState, -2, "KEY_LEFT_ARROW");
	lua_pushinteger(luaState, 264); // down
	lua_setfield(luaState, -2, "KEY_DOWN_ARROW");
	lua_pushinteger(luaState, 265); // up
	lua_setfield(luaState, -2, "KEY_UP_ARROW");
	*/

	//lua_pop(luaState, -1);
	lua_pushglobaltable(luaState);
	lua_pushnil(luaState);

	const char* _lol{ lua_tostring(luaState, 1) };

	globalVariables.clear();

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

void cs::ScriptComponent::CompileScript()
{
	if (luaOK && script != nullptr)
	{
		ResourceManager::Save<Script>(script->GetResourcePath(), script);

		ClearScript();
		CreateLuaState();

		if (luaOK) // CheckLua("Could not compile lua script", luaL_dostring(luaState, script->scriptText.c_str()))
			Debug::LogSuccess("Compilation Success: ", script->GetResourcePath());
	}
}

void cs::ScriptComponent::CreateLuaState()
{
	if (luaOK) // we don't want to create a new lua state whe we already have one active
		return;

	luaState = luaL_newstate();
	luaL_openlibs(luaState);

	lua_register(luaState, "get_component", &lua_functions::lua_GetComponent);

	luaOK = CheckLua("Could not read file", luaL_dofile(luaState, script->GetResourcePath().c_str()));

	GetGlobalTable();
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

void cs::ScriptComponent::Input(int keycode, int scancode, int action, int mods)
{
	if (!luaOK)
		return;

	if (lua_getglobal(luaState, "input") == LUA_TFUNCTION)
	{
		luaL_newmetatable(luaState, "event");
		lua_pushvalue(luaState, -1);
		lua_setfield(luaState, -2, "__index");
		lua_pushinteger(luaState, keycode);
		lua_setfield(luaState, -2, "keycode");
		lua_pushinteger(luaState, scancode);
		lua_setfield(luaState, -2, "scancode");
		lua_pushinteger(luaState, action);
		lua_setfield(luaState, -2, "action");
		lua_pushinteger(luaState, mods);
		lua_setfield(luaState, -2, "mods");
		IssueLua(lua_pcall(luaState, 1, 0, 0));
	}
}

void cs::ScriptComponent::Exit()
{
	if (!luaOK)
		return;

	if (lua_getglobal(luaState, "exit") == LUA_TFUNCTION)
		lua_pcall(luaState, 0, 0, 0);
}

void cs::ScriptComponent::OnCollisionEnter()
{
	if (!luaOK)
		return;

	if (lua_getglobal(luaState, "on_collision_exit") == LUA_TFUNCTION)
		lua_pcall(luaState, 0, 0, 0);
}

void cs::ScriptComponent::OnCollisionExit()
{
	if (!luaOK)
		return;

	if (lua_getglobal(luaState, "on_collision_exit") == LUA_TFUNCTION)
		lua_pcall(luaState, 0, 0, 0);
}

cs::Script* cs::ScriptComponent::GetScript() const
{
	return script;
}

void cs::ScriptComponent::SetScript(Script* newScript)
{
	if (newScript == nullptr || script == newScript)
		return;

	if (script != nullptr)
		ClearScript();

	script = newScript;

	CreateLuaState();
}

void cs::ScriptComponent::ClearScript()
{
	luaOK = false;
	lua_close(luaState);
}

cs::ScriptComponent::~ScriptComponent()
{
	script = nullptr;
	ClearScript();
}

void cs::ScriptComponent::IssueLua(int result)
{
	if (result != LUA_OK)
		Debug::LogIssue(lua_tostring(luaState, -1));
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

void cs::lua_tables::CreateTransform(lua_State* L, const char* name, TransformComponent& transform)
{
	luaL_newmetatable(L, name);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	CreateVector3(L, "position", transform.position);
	CreateVector3(L, "rotation", transform.rotation);
	CreateVector3(L, "scale", transform.scale);
}

void cs::lua_tables::CreateVector3(lua_State* L, const char* name, Vector3& vec)
{
	luaL_newmetatable(L, name);
	//lua_settable(L, -2);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pushnumber(L, vec.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, vec.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, vec.z);
	lua_setfield(L, -2, "z");
}

int cs::lua_functions::lua_GetComponent(lua_State* L)
{
	const std::string _componentName{ lua_tostring(L, 1) };

	if (_componentName.empty())
		return 0;

	void* _data;

	if (_componentName == "transform")
	{
		Debug::Log("We got the transform");
		return 0;
		//_data = &gameObject->GetComponent<TransformComponent>();
		//lua_tables::CreateTransform(luaState, "transform", gameObject->GetComponent<TransformComponent>());
	}
	/*else if (_componentName == "sprite_renderer")
		_data = &gameObject->GetComponent<SpriteRendererComponent>();
	else if (_componentName == "mesh_renderer")
		_data = &gameObject->GetComponent<MeshRendererComponent>();
	else if (_componentName == "camera")
		_data = &gameObject->GetComponent<CameraComponent>();
	else if (_componentName == "rigidbody")
		_data = &gameObject->GetComponent<RigidbodyComponent>();
	else if (_componentName == "staticbody")
		_data = &gameObject->GetComponent<StaticBodyComponent>();
	else if (_componentName == "b_spline")
		_data = &gameObject->GetComponent<BSpline>();*/
	else
		return 0;

	lua_pushlightuserdata(L, _data);
	return 1;
}
