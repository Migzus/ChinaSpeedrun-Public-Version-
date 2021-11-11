#pragma once

#include "Component.h"
#include "Resource.h"

#include <string>
#include <variant>
#include <vector>

struct lua_State;

namespace cs
{
	typedef std::variant<bool, int, std::string, float> LuaType;

	class Script : public Resource
	{
	public:
		friend class ResourceManager;

		virtual void Initialize() override;
	};

	class ScriptComponent : public Component
	{
	private:
		struct LuaVariable
		{
			std::string name;
			LuaType type;

			enum class Type
			{
				BOOL,
				INT,
				STRING,
				FLOAT
			};
		};

	public:
		ScriptComponent();

		virtual void Init() override;
		virtual void ImGuiDrawComponent() override;
		
		void GetGlobalTable();

		void Start();
		void Update();
		void Input(char eventKey);
		void Exit();

		void SetScript(Script* newScript);

		~ScriptComponent();

	private:
		lua_State* luaState;
		Script* script;
		bool luaOK;

		std::vector<LuaVariable> globalVariables;

		bool CheckLua(const char* errorMessage, int result);
	};
}
