#include "EngineEditor.h"

#include "ChinaEngine.h"
#include "World.h"

cs::EngineEditor::Playmode cs::EngineEditor::mode;

const cs::EngineEditor::Playmode& cs::EngineEditor::GetPlaymodeState()
{
	return mode;
}

void cs::EngineEditor::SetPlaymode(const Playmode newPlaymode)
{
	mode = newPlaymode;

	ChinaEngine::world.Start();
}
