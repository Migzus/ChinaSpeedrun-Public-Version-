#pragma once

#include "Component.h"
#include "Resource.h"

#include "GameObject.h"

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

		std::string scriptText;

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
		void CompileScript();
		void CreateLuaState();

		void Start();
		void Update();
		void Input(char eventKey);
		void Exit();

		Script* GetScript() const;
		void SetScript(Script* newScript);
		void ClearScript();

		~ScriptComponent();

	private:
		lua_State* luaState;
		Script* script;
		bool luaOK;

		std::vector<LuaVariable> globalVariables;

		bool CheckLua(const char* errorMessage, int result);
	};

	class TransformComponent;

	namespace lua_functions
	{
		int lua_GetComponent(lua_State* L);
	}

	namespace lua_tables
	{
		void CreateTransform(lua_State* L, const char* name, TransformComponent& transform);
		void CreateVector3(lua_State* L, const char* name, Vector3& vec);
	}
}
