#include "Signal.h"

bool cs::Signal::HasSignal(const std::string& signalName)
{
	return signals.find(signalName) != signals.end();
}

template<class Class, typename ...Any>
cs::Signal::SignalCombo<Class, Any...>::SignalCombo(Class* oRef, FuncPtr fRef) :
	objectRef{ oRef }, functionRef{ fRef }
{}

template<class Class, typename ...Any>
bool cs::Signal::SignalCombo<Class, Any...>::operator==(const SignalCombo<Class, Any...>& other)
{
	return objectRef == other.objectRef && functionRef == other.functionRef;
}
