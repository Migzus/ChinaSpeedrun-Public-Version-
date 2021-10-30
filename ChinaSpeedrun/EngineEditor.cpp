#include "Editor.h"

#include "ChinaEngine.h"
#include "World.h"
#include "Input.h"
#include "Camera.h"
#include "EditorCamera.h"

cs::editor::EngineEditor::Playmode cs::editor::EngineEditor::mode;
cs::editor::EditorCamera* cs::editor::EngineEditor::editorCamera;

const cs::editor::EngineEditor::Playmode& cs::editor::EngineEditor::GetPlaymodeState()
{
	return mode;
}

void cs::editor::EngineEditor::SetPlaymode(const Playmode newPlaymode)
{
	mode = newPlaymode;

	switch (mode)
	{
	case cs::editor::EngineEditor::Playmode::EDITOR:
		ChinaEngine::world.mainCamera = editorCamera;
		ChinaEngine::world.Stop();
		break;
	case cs::editor::EngineEditor::Playmode::PLAY:
		ChinaEngine::world.Start();
		break;
	case cs::editor::EngineEditor::Playmode::PAUSE:
		ChinaEngine::world.Stop();
		break;
	}
}

void cs::editor::EngineEditor::Start()
{
	Input::AddMapping("editor_forward", GLFW_KEY_W);
	Input::AddMapping("editor_backward", GLFW_KEY_S);
	Input::AddMapping("editor_left", GLFW_KEY_A);
	Input::AddMapping("editor_right", GLFW_KEY_D);
	Input::AddMapping("editor_up", GLFW_KEY_Q);
	Input::AddMapping("editor_down", GLFW_KEY_E);

	Input::AddMapping("editor_translate", GLFW_KEY_1);
	Input::AddMapping("editor_rotate", GLFW_KEY_2);
	Input::AddMapping("editor_scale", GLFW_KEY_3);
	Input::AddMapping("editor_local", GLFW_KEY_LEFT_CONTROL);
	Input::AddMapping("editor_snap", GLFW_KEY_LEFT_SHIFT);

	editorCamera = new EditorCamera;

	Camera::CalculatePerspective(*editorCamera);

	SetPlaymode(Playmode::EDITOR);
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
	// might be useful to remove mappings when disableing the editor
	Input::RemoveMapping("editor_forward", GLFW_KEY_W);
	Input::RemoveMapping("editor_backward", GLFW_KEY_S);
	Input::RemoveMapping("editor_left", GLFW_KEY_A);
	Input::RemoveMapping("editor_right", GLFW_KEY_D);
	Input::RemoveMapping("editor_up", GLFW_KEY_Q);
	Input::RemoveMapping("editor_down", GLFW_KEY_E);

	Input::RemoveMapping("editor_translate", GLFW_KEY_1);
	Input::RemoveMapping("editor_rotate", GLFW_KEY_2);
	Input::RemoveMapping("editor_scale", GLFW_KEY_3);
	Input::RemoveMapping("editor_local", GLFW_KEY_LEFT_CONTROL);
	Input::RemoveMapping("editor_snap", GLFW_KEY_LEFT_SHIFT);

	delete editorCamera;
}
