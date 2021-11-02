#pragma once

#include <unordered_map>
#include "Debug.h"

namespace cs
{
	typedef int func(void);

	struct ConnectionPair
	{
		void* methodRef;

		ConnectionPair(void* method);

		template<typename ...Any>
		void Activate(const Any ...rest);
	};

	class Signal
	{
	public:
		// Emit the desired signal, with any arguments
		template <class C, typename ...Any>
		static void EmitSignal(const char* signalName, C* target, const Any ...rest);
		// Connect a signal of any name, give the reference to the instance and its method.
		// Once EmitSignal is called every function with this name and target will be called.
		template <class C, typename ...Any>
		static void Connect(const char* signalName, C* target, void(C::* method)(Any ...rest));
		template <class C, typename ...Any>
		static void Disconnect(const char* signalName, C* target, void(C::* method)(Any ...rest));
		template <class C, typename ...Any>
		static void IsConnected(const char* signalName, C* target, void(C::* method)(Any ...rest));

	private:
		static std::unordered_map<std::string, ConnectionPair> signals;
	};
	
	template<typename ...Any>
	inline void ConnectionPair::Activate(const Any ...rest)
	{
#ifdef CHAIKA_EDITOR
		Debug::LogIssue("Function references do not match the targeted function!");
		return;
#endif // CHAIKA_EDITOR

		func* _f{ (func*)methodRef };
		_f(rest...);
	}

	template <class C, typename ...Any>
	void Signal::EmitSignal(const char* signalName, C* target, const Any ...rest)
	{
		signals.at(signalName + target).Activate(rest...);
	}

	template<class C, typename ...Any>
	inline void Signal::Connect(const char* signalName, C* target, void(C::* method)(Any ...rest))
	{
		signals[signalName + target] = ConnectionPair(method);
	}
	
	template<class C, typename ...Any>
	inline void Signal::Disconnect(const char* signalName, C* target, void(C::* method)(Any...rest))
	{
		signals.erase(signalName + target);
	}
	
	template<class C, typename ...Any>
	inline void Signal::IsConnected(const char* signalName, C* target, void(C::* method)(Any...rest))
	{
		return signals.find(signalName + target) != signals.end();
	}
}
