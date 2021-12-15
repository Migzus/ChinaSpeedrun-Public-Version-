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
		
		/**
		* Gets all the variables in the lua instance
		*/
		void GetGlobalTable();
		/**
		* Compile the lua script
		*/
		void CompileScript();
		/**
		* Creates a lua state (Always called automatically)
		*/
		void CreateLuaState();

		/**
		* Called on start scene play
		*/
		void Start();
		/**
		* Call the lua script's update every frame
		*/
		void Update();
		/**
		* Gives Input to the lua script
		*/
		void Input(int keycode, int scancode, int action, int mods);
		/**
		* Called once the scene stops
		*/
		void Exit();

		/**
		* Called by a local box2d component, once something enters the collider
		*/
		void OnCollisionEnter();
		/**
		* Called by a local box2d component, once something exits the collider
		*/
		void OnCollisionExit();

		/**
		* Get the current attached script
		*/
		Script* GetScript() const;
		/**
		* Set a new script
		*/
		void SetScript(Script* newScript);
		/**
		* Clear the current script
		*/
		void ClearScript();

		~ScriptComponent();

	private:
		lua_State* luaState;
		Script* script;
		bool luaOK;

		std::vector<LuaVariable> globalVariables;

		void IssueLua(int result);
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
