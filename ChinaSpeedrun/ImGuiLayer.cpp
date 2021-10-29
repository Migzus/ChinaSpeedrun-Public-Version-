#include "ImGuiLayer.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "ImGuizmo.h"

#include "Mathf.h"
#include "GameObject.h"
#include "Time.h"

#include "ChinaEngine.h"
#include "Editor.h"

//using namespace cs::editor;

void cs::ImGuiLayer::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& _io{ ImGui::GetIO() }; (void)_io;
}

void cs::ImGuiLayer::Begin()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

void cs::ImGuiLayer::Step()
{
    static GameObject* _activeObject{ nullptr };

    ImGui::ShowDemoWindow();

    if (ImGui::Begin("Hierarchy"))
    {
        if (ImGui::TreeNode("Main Scene"))
        {
            for (GameObject* object : ChinaEngine::world.GetObjects())
            {
                ImGui::Text(object->name.c_str());
                if (ImGui::IsItemClicked())
                    _activeObject = object;
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();

	if (ImGui::Begin("Inspector"))
	{
		if (_activeObject != nullptr)
		{
            ImGui::Checkbox("", &_activeObject->active);
            ImGui::SameLine();
			ImGui::Text(_activeObject->name.c_str());

            _activeObject->EditorDrawComponents();
		}
	}
	ImGui::End();

    if (ImGui::Begin("Top Bar"))
    {
        switch (editor::EngineEditor::GetPlaymodeState())
        {
        case editor::EngineEditor::Playmode::EDITOR:
            ImGui::Button("Play");
            if (ImGui::IsItemClicked())
                editor::EngineEditor::SetPlaymode(editor::EngineEditor::Playmode::PLAY);
            break;
        case editor::EngineEditor::Playmode::PLAY:
            ImGui::Button("Pause");
            if (ImGui::IsItemClicked())
                editor::EngineEditor::SetPlaymode(editor::EngineEditor::Playmode::PAUSE);
            break;
        case editor::EngineEditor::Playmode::PAUSE:
            ImGui::Button("Continue");
            if (ImGui::IsItemClicked())
                editor::EngineEditor::SetPlaymode(editor::EngineEditor::Playmode::PLAY);
            break;
        }

        ImGui::SameLine();
        ImGui::Button("Stop");
        if (ImGui::IsItemClicked())
            editor::EngineEditor::SetPlaymode(editor::EngineEditor::Playmode::EDITOR);
    }
    ImGui::End();

	if (ImGui::Begin("Profiler"))
		ImGui::Text("Delta Time: %f", Time::deltaTime);
	ImGui::End();
}

void cs::ImGuiLayer::End()
{
    ImGui::Render();
}

void cs::ImGuiLayer::SetStyle()
{
    ImGuiStyle* _style{ &ImGui::GetStyle() };
}

bool cs::ImGuiLayer::BeginButtonDropDown(const char* label, ImVec2 buttonSize)
{
    ImGui::SameLine(0.f, 0.f);

    ImVec2 _pos{ ImGui::GetWindowPos() };
    
    float x = ImGui::GetCursorPosX();
    float y = ImGui::GetCursorPosY();

    ImVec2 size(20, buttonSize.y);
    bool pressed{ ImGui::Button("##", size) };

    // Arrow
    ImVec2 center(_pos.x + x + 10, _pos.y + y + buttonSize.y / 2);
    float r = 8.0f;
    center.y -= r * 0.25f;
    /*ImVec2 a = center + ImVec2(0, 1) * r;
    ImVec2 b = center + ImVec2(-0.866f, -0.5f) * r;
    ImVec2 c = center + ImVec2(0.866f, -0.5f) * r;

    window->DrawList->AddTriangleFilled(a, b, c, ImGui::GetColorU32(ImGuiCol_Text));*/

    // Popup

    ImVec2 popupPos;

    popupPos.x = _pos.x + x - buttonSize.x;
    popupPos.y = _pos.y + y + buttonSize.y;

    ImGui::SetNextWindowPos(popupPos);

    if (pressed)
        ImGui::OpenPopup(label);

    if (ImGui::BeginPopup(label))
    {
        //ImGui::PushStyleColor(ImGuiCol_FrameBg, style.Colors[ImGuiCol_Button]);
        //ImGui::PushStyleColor(ImGuiCol_WindowBg, style.Colors[ImGuiCol_Button]);
        //ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, style.Colors[ImGuiCol_Button]);
        return true;
    }

    return false;
}

void cs::ImGuiLayer::EndButtonDropDown()
{
    ImGui::PopStyleColor(3);
    ImGui::EndPopup();
}

