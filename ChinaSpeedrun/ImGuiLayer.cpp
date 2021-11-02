#include "Editor.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "ImGuizmo.h"

#include <glm/gtx/quaternion.hpp>
#include "Mathf.h"
#include "GameObject.h"
#include "Time.h"

#include "Transform.h"
#include "VulkanEngineRenderer.h"
#include "ChinaEngine.h"
#include "World.h"
#include "Camera.h"

cs::editor::ImGuiLayer::ImGuiLayer(EngineEditor* root) :
    editorRoot{ root }, activeObject{ nullptr }, isManipulating{ false }, isWindowActive{ false }
{}

void cs::editor::ImGuiLayer::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& _io{ ImGui::GetIO() }; (void)_io;
}

void cs::editor::ImGuiLayer::Begin()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

void cs::editor::ImGuiLayer::Step()
{
    bool _gizmoWindow{ true };

    isWindowActive = false;

    if (ImGui::Begin("Gizmos", &_gizmoWindow, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground))
    {
        int _width, _height;
        ChinaEngine::renderer.GetViewportSize(_width, _height);

        ImGui::SetWindowPos({ 0.0f, 0.0f });
        ImGui::SetWindowSize({ static_cast<float>(_width), static_cast<float>(_height) });

        if (activeObject != nullptr && activeObject->HasComponent<TransformComponent>())
        {
            TransformComponent& _transform{ activeObject->GetComponent<TransformComponent>() };
            Matrix4x4 _viewMatrix{ Camera::GetViewMatrix(*ChinaEngine::world.mainCamera) }, _projectionMatrix{ Camera::GetProjectionMatrix(*ChinaEngine::world.mainCamera) };
            Matrix4x4& _transformMatrix{ Transform::GetMatrixTransform(_transform) };

            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(0.0f, 0.0f, static_cast<float>(_width), static_cast<float>(_height));

            _projectionMatrix[1][1] *= -1.0f;

            ImGuizmo::Manipulate(glm::value_ptr(_viewMatrix), glm::value_ptr(_projectionMatrix),
                editorRoot->GetOperationState(), editorRoot->GetMode(), glm::value_ptr(_transformMatrix));

            isManipulating = ImGuizmo::IsUsing();

            if (isManipulating)
            {
                Vector3 _position{}, _rotation{}, _scale{};
                //Mathf::DecomposeMatrix(_transformMatrix, _position, _rotation, _scale); // This doesn't work...

                ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(_transformMatrix), &_position[0], &_rotation[0], &_scale[0]);

                _transform.position = _position;
                _transform.rotationDegrees = _rotation;
                _transform.scale = _scale;
            }
        }
    }
    ImGui::End();

    if (ImGui::Begin("Hierarchy"))
    {
        if (ImGui::TreeNode("Main Scene"))
        {
            for (GameObject* object : ChinaEngine::world.GetObjects())
            {
                ImGui::Text(object->name.c_str());
                if (ImGui::IsItemClicked())
                    activeObject = object;
            }
            ImGui::TreePop();
        }

        IsWindowHovered();
    }
    ImGui::End();

	if (ImGui::Begin("Inspector"))
	{
		if (activeObject != nullptr)
		{
            ImGui::Checkbox("", &activeObject->active);
            ImGui::SameLine();
			ImGui::Text(activeObject->name.c_str());

            activeObject->EditorDrawComponents();
		}

        IsWindowHovered();
	}
	ImGui::End();

    if (ImGui::Begin("Top Bar"))
    {
        switch (editorRoot->GetPlaymodeState())
        {
        case EngineEditor::Playmode::EDITOR:
            ImGui::Button("Play");
            if (ImGui::IsItemClicked())
                editorRoot->SetPlaymode(EngineEditor::Playmode::PLAY);
            break;
        case editor::EngineEditor::Playmode::PLAY:
            ImGui::Button("Pause");
            if (ImGui::IsItemClicked())
                editorRoot->SetPlaymode(EngineEditor::Playmode::PAUSE);
            break;
        case EngineEditor::Playmode::PAUSE:
            ImGui::Button("Continue");
            if (ImGui::IsItemClicked())
                editorRoot->SetPlaymode(EngineEditor::Playmode::PLAY);
            break;
        }

        ImGui::SameLine();
        ImGui::Button("Stop");
        if (ImGui::IsItemClicked())
            editorRoot->SetPlaymode(EngineEditor::Playmode::EDITOR);

        IsWindowHovered();
    }
    ImGui::End();

    if (ImGui::Begin("Profiler"))
    {
        ImGui::Text("Delta Time: %f", Time::deltaTime);

        const auto& _status{ ChinaEngine::renderer.GetStatus() };

        ImGui::Text("Index Buffer");
        ImGui::SameLine();
        ImGui::ProgressBar(_status.indexDataFractionSize);
        ImGui::Text("%f kB", (float)(*_status.indexDataSize) * 0.0001f);

        ImGui::Text("Vertex Buffer");
        ImGui::SameLine();
        ImGui::ProgressBar(_status.vertexDataFractionSize);
        ImGui::Text("%f kB", (float)(*_status.vertexDataSize) * 0.0001f);

        IsWindowHovered();
    }
	ImGui::End();

    if (ImGui::Begin("Debugger"))
    {
        Debug::ImGuiDrawMessages();
        IsWindowHovered();
    }
    ImGui::End();
}

void cs::editor::ImGuiLayer::End()
{
    ImGui::Render();
}

void cs::editor::ImGuiLayer::SetStyle()
{
    ImGuiStyle* _style{ &ImGui::GetStyle() };
}

const bool& cs::editor::ImGuiLayer::IsManipulating() const
{
    return isManipulating;
}

const bool& cs::editor::ImGuiLayer::IsInteractingWithWindow() const
{
    return isWindowActive;
}

void cs::editor::ImGuiLayer::IsWindowHovered()
{
    isWindowActive |= ImGui::IsWindowHovered(
        ImGuiHoveredFlags_AllowWhenBlockedByActiveItem |
		ImGuiHoveredFlags_RootAndChildWindows);
}
