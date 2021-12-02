#include "BSpline.h"

//#include "Draw.h"
#include "GameObject.h"
#include "Transform.h"

#include "Mesh.h"
#include <unordered_map>

cs::BSpline::Point3D::Point3D() :
    position{ Vector3(0.0f) }, tangentIn{ Vector3(0.0f) }, tangentOut{ Vector3(0.0f) }
{}

cs::BSpline::Point3D::Point3D(const Vector3& position, const Vector3& tangent) :
    position{ position }, tangentIn{ -tangent }, tangentOut{ tangent }
{}

cs::BSpline::Point3D::Point3D(const Vector3& position, const Vector3& tangentIn, const Vector3& tangentOut) :
    position{ position }, tangentIn{ tangentIn }, tangentOut{ tangentOut }
{}

cs::BSpline::BSpline() :
    loop{ false }, subdivisions{ 10 }, matrixWarp{ nullptr }, size{ 0 }
{}

cs::BSpline::BSpline(std::vector<Point3D> points) :
    loop{ false }, points{ points }, subdivisions{ 10 }, matrixWarp{ nullptr }, size{ 0 }
{}

cs::BSpline::BSpline(std::vector<Point3D> points, size_t subDivs) :
    loop{ false }, points{ points }, subdivisions{ subDivs }, matrixWarp{ nullptr }, size{ 0 }
{}

void cs::BSpline::Init()
{
    if (gameObject->HasComponent<TransformComponent>())
        matrixWarp = &Transform::GetMatrixTransform(gameObject->GetComponent<TransformComponent>());
    else
        matrixWarp = new Matrix4x4(1.0f);
}

void cs::BSpline::ImGuiDrawComponent()
{
    if (ImGui::TreeNodeEx("B-Spline", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::TreeNode("Points"))
        {
            ImGui::Checkbox("Loop", &loop);
            if (ImGui::DragInt("Size", &size, 1.0f, 0, 100000))
                points.resize(size);

            for (uint32_t i{ 0 }; i < points.size(); i++)
            {
                if (ImGui::TreeNode(std::to_string(i).c_str()))
                {
                    ImGui::DragFloat3("Position", &points[i].position[0], 0.05f);
                    ImGui::DragFloat3("Tangent ", &points[i].tangentIn[0], 0.05f);

                    points[i].tangentOut = -points[i].tangentIn;

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
        
        ImGui::TreePop();
    }
}

void cs::BSpline::UpdateMesh()
{
    size_t _index{ 0 };
    const float _padding{ 1.0f / static_cast<float>(subdivisions) };
    for (float i{ 0 }; i < points.size() - 1.0f; i += _padding)
    {
        Vector3& _startPoint{ item->mesh->vertices[_index].position };
        InterpolateNoMatrix(i, _startPoint);
        Vector3& _endPoint{ item->mesh->vertices[_index + 1].position };
        InterpolateNoMatrix(i + _padding, _endPoint);
        item->mesh->vertices[_index + 2].position = _endPoint;
        _index += 3;
    }

    item->UpdateMesh();
}

void cs::BSpline::MakeDrawLine()
{
    std::vector<Vector3> _linePoints;
    const float _padding{ 1.0f / static_cast<float>(subdivisions) };

    for (float i{ 0 }; i < points.size() - 1.0f; i += _padding)
    {
        Vector3 _point{ Vector3(0.0f) };
        InterpolateNoMatrix(i, _point);
        _linePoints.push_back(_point);
    }

    _linePoints.push_back(points.back().position);

    item = Draw::Line(_linePoints, { Color::magenta }, Draw::DrawMode::SOLID);
}

// the float value can be 0.0f to the number of "points - 1" (example: 4.0f, and 5 points; we would end up on the fifth point)
void cs::BSpline::Interpolate(const float& value, Vector3& point)
{
    InterpolateNoMatrix(value, point);
    const Vector4 _pointOut{ *matrixWarp * Vector4(point, 1.0f) };
    point.x = _pointOut.x;
    point.y = _pointOut.y;
    point.z = _pointOut.z;
}

void cs::BSpline::InterpolateNoMatrix(const float& value, Vector3& point)
{
    size_t _roundVal{ (size_t)value };
    Point3D* _p0{ &points[_roundVal] };
    Point3D* _p1{ &points[Mathf::Clamp(_roundVal + 1, (size_t)0, points.size() - 1)] };
    float _rest{ value - _roundVal }, _in{ 1.0f - _rest };

    point = _in * _in * _in * _p0->position +
        _in * _in * _rest * (_p0->tangentOut + _p0->position) * 3.0f +
        _in * _rest * _rest * (_p1->tangentIn + _p1->position) * 3.0f +
        _rest * _rest * _rest * _p1->position;
}
