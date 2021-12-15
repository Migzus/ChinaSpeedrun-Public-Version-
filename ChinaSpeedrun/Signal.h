#pragma once

#include "Debug.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <variant>

#include "Editor.h"

namespace cs
{
	class Signal
	{
	public:
		/**
		* Emit the desired signal, with any arguments
		*/
		template <typename ...Any>
		void EmitSignal(const std::string& signalName, Any ...rest);
		/**
		* Connect a signal of any name, give the reference to the instance and its method.
		* Once EmitSignal is called every function with this name and target will be called.
		*/
		template<class C, typename ...Any>
		void Connect(const std::string& signalName, C* object, void(__thiscall C::* functionAddress)(Any...));
		/**
		* Disconnect an existing singal.
		*/
		template<class C, typename ...Any>
		void Disconnect(const std::string& signalName, C* object, void(__thiscall C::* functionAddress)(Any...));
		/**
		* Checks if the spesified signal exists on the current referenced class context.
		*/
		template<class C, typename ...Any>
		bool IsConnected(const std::string& signalName, C* object, void(__thiscall C::* functionAddress)(Any...));

		/**
		* Check if a signal has already been created
		*/
		bool HasSignal(const std::string& signalName);

	private:
		template<class Class, typename ...Any>
		struct SignalCombo
		{
			typedef void (Class::* FuncPtr)(Any...);

			Class* objectRef;
			FuncPtr functionRef;

			SignalCombo(Class* oRef, FuncPtr fRef);

			bool operator==(const SignalCombo& other);
		};

		// alright i have to explain this one...
		// we have the name of the signal			(that is not related to a spesific object)
		// then we have the address of the object	(this is spesific to an object, but also acts as a helper to know what function address to call)
		// then we have the function address
		std::unordered_map<std::string, std::vector<SignalCombo<class null>>> signals;
	};
	
	template<typename ...Any>
	inline void Signal::EmitSignal(const std::string& signalName, Any ...rest)
	{
#ifdef CHAIKA_EDITOR
		Debug::LogInfo("Emitting: ", signalName);
		
		if (!HasSignal(signalName))
		{
			Debug::LogIssue("Cannot emit \"", signalName, "\". The signal does not exist.");
			return;
		}
#endif // CHAIKA_EDITOR

		for (auto& combo : signals[signalName])
			(combo.objectRef->*combo.functionRef)(rest...);
	}

	template<class C, typename ...Any>
	inline void Signal::Connect(const std::string& signalName, C* object, void(__thiscall C::* functionAddress)(Any...))
	{
		// if the name in the map doesn't exist, it will be automatically created
		signals[signalName].push_back(SignalCombo<C, Any...>(object, functionAddress));
	}
	
	template<class C, typename ...Any>
	inline void Signal::Disconnect(const std::string& signalName, C* object, void(__thiscall C::* functionAddress)(Any...))
	{
		auto& _funcRefs{ signals[signalName] };

#ifdef CHAIKA_EDITOR
		if (!HasSignal(signalName))
		{
			Debug::LogIssue("Cannot disconnect \"", signalName, "\". The signal does not exist.");
			return;
		}

		if (std::find(_funcRefs.begin(), _funcRefs.end(), SignalCombo<C, Any...>(object, functionAddress)) == _funcRefs.end())
		{
			Debug::LogIssue("Cannot disconnect \"", signalName, "\". The object reference does not exist.");
			return;
		}
#endif // CHAIKA_EDITOR
		
		_funcRefs.erase(std::find(_funcRefs.begin(), _funcRefs.end(), SignalCombo<C, Any...>(object, functionAddress)));

		if (_funcRefs.empty())
			signals.erase(signalName);
	}
	
	template<class C, typename ...Any>
	inline bool Signal::IsConnected(const std::string& signalName, C* object, void(__thiscall C::* functionAddress)(Any...))
	{
		if (!HasSignal(signalName))
			return false;

		auto& _funcRefs{ signals[signalName] };
		return std::find(_funcRefs.begin(), _funcRefs.end(), SignalCombo<C, Any...>(object, functionAddress)) != _funcRefs.end();
	}
}
