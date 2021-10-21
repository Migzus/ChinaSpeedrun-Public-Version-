#include "Signal.h"

std::unordered_map<std::string, cs::ConnectionPair> cs::Signal::signals;

cs::ConnectionPair::ConnectionPair(void* method) :
	methodRef{ method }
{}
