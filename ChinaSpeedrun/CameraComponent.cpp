#include "CameraComponent.h"
#include "imgui.h"

//cs::CameraComponent* cs::CameraComponent::currentActiveCamera;

void cs::CameraComponent::Init()
{

}

void cs::CameraComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const char* _options[]{ "Orthographic", "Perspective" };
		ImGui::Combo("Projection", (int*)&projection, _options, IM_ARRAYSIZE(_options));
		
		switch (projection)
		{
		case Projection::ORTHOGRAPHIC:
			ImGui::DragFloat("Left", &leftPlane);
			ImGui::DragFloat("Right", &rightPlane);
			ImGui::DragFloat("Up", &topPlane);
			ImGui::DragFloat("Down", &bottomPlane);
			break;
		case Projection::PERSPECTIVE:
			ImGui::DragFloat("Field of View", &fov, 1.0f, 0.1f, 179.0f);
			break;
		}

		ImGui::DragFloat("Near Plane", &nearPlane, 1.0f, 0.001f);
		ImGui::DragFloat("Far Plane", &farPlane, 1.0f);

		ImGui::TreePop();
	}
}
