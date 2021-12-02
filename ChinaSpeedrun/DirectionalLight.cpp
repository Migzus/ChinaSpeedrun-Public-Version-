#include "DirectionalLight.h"

#include "imgui.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mathf.h"
#include <vector>

cs::DirectionalLight::DirectionalLight() :
	intensity{ 1.0f }, lightColor{ Color::white }
{}

void cs::DirectionalLight::Init()
{}

void cs::DirectionalLight::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Directional Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::ColorEdit4("Light Color", &lightColor.r);
		ImGui::DragFloat("Intensity", &intensity);

		ImGui::TreePop();
	}
}

void cs::DirectionalLight::MakeRayLines()
{
	std::vector<Vector3> _points;
	const size_t _rayCount{ 10 };
	const float _rayLength{ 1.0f };
	const float _radius{ 0.25f };
	const float _rotOffset{ Mathf::TAU / (float)_rayCount };
	TransformComponent& _transform{ gameObject->GetComponent<TransformComponent>() };
	Transform::CalculateMatrix(_transform);
	Matrix4x4 _orientation{ gameObject->HasComponent<TransformComponent>() ? Transform::GetMatrixTransform(_transform) : Matrix4x4(1.0f) };

	for (size_t i{ 0 }; i < _rayCount + 1; i++)
	{
		const float _currentRotation{ _rotOffset * (float)i };
		Vector3 _pointEdit{ Vector3(cos(_currentRotation), 0.0, sin(_currentRotation)) * _radius };

		_points.push_back(_orientation * Vector4(_pointEdit, 1.0f));
		_pointEdit.y -= _rayLength;
		_points.push_back(_orientation * Vector4(_pointEdit, 1.0f));
	}

	Draw::Line(_points, { lightColor }, Draw::DrawMode::DASHED, gameObject->GetScene());
}
