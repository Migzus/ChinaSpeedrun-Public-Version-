#include "Editor.h"

#include "ChinaEngine.h"
#include "World.h"

cs::editor::EngineEditor::Playmode cs::editor::EngineEditor::mode;
cs::editor::EditorCamera* cs::editor::EngineEditor::editorCamera;

const cs::editor::EngineEditor::Playmode& cs::editor::EngineEditor::GetPlaymodeState()
{
	return mode;
}

void cs::editor::EngineEditor::SetPlaymode(const Playmode newPlaymode)
{
	mode = newPlaymode;

	ChinaEngine::world.Start();
}

void cs::editor::EngineEditor::Start()
{

}

void cs::editor::EngineEditor::Update()
{
	if (mode == Playmode::EDITOR)
	{
		editorCamera->Update();
	}
}

void cs::editor::EngineEditor::Exit()
{

}
