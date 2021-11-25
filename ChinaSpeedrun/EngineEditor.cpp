#include "Editor.h"

#include "imgui.h"
#include "ImGuizmo.h"
#include "ChinaEngine.h"
#include "PhysicsServer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Input.h"
#include "Camera.h"
#include "CameraComponent.h"

#include "Debug.h"

const ImGuizmo::OPERATION& cs::editor::EngineEditor::GetOperationState()
{
	return operation;
}

const ImGuizmo::MODE& cs::editor::EngineEditor::GetMode()
{
	return operationMode;
}

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
	{
		SceneManager::mainCamera = editorCamera;

		if (SceneManager::HasScenes() && SceneManager::GetCurrentScene()->GetPhysicsServer() != nullptr)
			SceneManager::GetCurrentScene()->GetPhysicsServer()->Reset();
		break;
	}
	case cs::editor::EngineEditor::Playmode::PLAY:
	{
		Scene* _scene{ SceneManager::GetCurrentScene() };

		if (_scene != nullptr)
		{
			auto _camera{ SceneManager::GetRegistry().view<CameraComponent>() };

			if (_camera.empty())
			{
				Debug::LogError("Cannot run a scene with no cameras. Add a camera component!");
				mode = Playmode::EDITOR;
			}
			else
			{
				SceneManager::mainCamera = &SceneManager::GetRegistry().get<CameraComponent>(_camera.front());
				_scene->Start();
			}
		}
		else
		{
			Debug::LogError("You do not have any scenes active.");
			mode = Playmode::EDITOR;
		}

		break;
	}
	case cs::editor::EngineEditor::Playmode::PAUSE:
	{
		break;
	}
	}
}

cs::GameObject* cs::editor::EngineEditor::GetSelectedGameObject() const
{
	return uiLayer->activeObject;
}

void cs::editor::EngineEditor::SetSelectedGameObject(GameObject* gameObject)
{
	uiLayer->activeObject = gameObject;
}

void cs::editor::EngineEditor::Start()
{
	Input::AddMapping("editor_forward", GLFW_KEY_W);
	Input::AddMapping("editor_backward", GLFW_KEY_S);
	Input::AddMapping("editor_left", GLFW_KEY_A);
	Input::AddMapping("editor_right", GLFW_KEY_D);
	Input::AddMapping("editor_up", GLFW_KEY_Q);
	Input::AddMapping("editor_down", GLFW_KEY_E);

	Input::AddMapping("editor_translate", GLFW_KEY_W);
	Input::AddMapping("editor_rotate", GLFW_KEY_E);
	Input::AddMapping("editor_scale", GLFW_KEY_R);
	Input::AddMapping("editor_mode_switch", GLFW_KEY_LEFT_CONTROL);
	Input::AddMapping("editor_snap", GLFW_KEY_LEFT_SHIFT);

	editorCamera = new EditorCamera(this);
	//editorCamera->projection = CameraBase::Projection::ORTHOGRAPHIC;
	//editorCamera->SetExtents(42.0f, 80.0f);
	Camera::CalculateProjection(*editorCamera);

	uiLayer = new ImGuiLayer(this);

	SetPlaymode(Playmode::EDITOR);
	operation = ImGuizmo::TRANSLATE;
	operationMode = ImGuizmo::LOCAL;
}

void cs::editor::EngineEditor::Update()
{
	if (Input::GetActionPressed("editor_mode_switch"))
		operationMode = operationMode == ImGuizmo::WORLD ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

	// by moving the ui layer (editor ui) here, we have more control over when things are executed
	uiLayer->Begin();
	uiLayer->Step();

	if (mode == Playmode::EDITOR)
	{
		editorCamera->Update();

		if (!Input::GetMouseHeld(1))
		{
			if (Input::GetActionPressed("editor_translate"))
				operation = ImGuizmo::TRANSLATE;
			else if (Input::GetActionPressed("editor_rotate"))
				operation = ImGuizmo::ROTATE;
			else if (Input::GetActionPressed("editor_scale"))
				operation = ImGuizmo::SCALE;
		}
	}

	uiLayer->End();
}

void cs::editor::EngineEditor::Exit()
{
	// might be useful to remove mappings when disableing the editor
	Input::RemoveMapping("editor_forward");
	Input::RemoveMapping("editor_backward");
	Input::RemoveMapping("editor_left");
	Input::RemoveMapping("editor_right");
	Input::RemoveMapping("editor_up");
	Input::RemoveMapping("editor_down");

	Input::RemoveMapping("editor_translate");
	Input::RemoveMapping("editor_rotate");
	Input::RemoveMapping("editor_scale");
	Input::RemoveMapping("editor_mode_switch");
	Input::RemoveMapping("editor_snap");

	delete editorCamera;
	delete uiLayer;
}
