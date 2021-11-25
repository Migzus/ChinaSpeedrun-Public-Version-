#include "BSpline.h"

#include "Draw.h"
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
    subdivisions{ 10 }, matrixWarp{ nullptr }, size{ 0 }
{}

cs::BSpline::BSpline(std::vector<Point3D> points) :
    points{ points }, subdivisions{ 10 }, matrixWarp{ nullptr }, size{ 0 }
{}

cs::BSpline::BSpline(std::vector<Point3D> points, size_t subDivs) :
    points{ points }, subdivisions{ subDivs }, matrixWarp{ nullptr }, size{ 0 }
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

    Draw::Line(_linePoints, { Color::cyan }, Draw::DrawMode::SOLID);
}

/*void cs::BSpline::Start()
{
    DrawCurve();
}

void cs::BSpline::Draw()
{
    glBindVertexArray(VAO);
    glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, transform.GetTransfomationMatrix().constData());
    glDrawArrays(GL_LINES, 0, mesh->vertices.size());
}

void cs::BSpline::DrawCurve()
{
    if (!visible)
        return;

    const float _offset{ 1.0f / (float)subdivisions };
    QVector3D _point;

    mesh = new Mesh;

    for (float i{ 0.0f }; i < (float)points.size() - 1.0f; i += _offset)
    {
        mesh->vertices.push_back(Vertex(_point - transform.position, Color::cyan, { 1.0f, 1.0f }));
        Interpolate(i, _point);
        mesh->vertices.push_back(Vertex(_point - transform.position, Color::cyan, { 1.0f, 1.0f }));
    }

    for (auto point : points)
    {
        mesh->vertices.push_back(Vertex(point.position, Color::lime, { 1.0f, 1.0f }));
        mesh->vertices.push_back(Vertex(point.tangentIn + point.position, Color::lime, { 1.0f, 1.0f }));

        mesh->vertices.push_back(Vertex(point.position, Color::yellow, { 1.0f, 1.0f }));
        mesh->vertices.push_back(Vertex(point.tangentOut + point.position, Color::yellow, { 1.0f, 1.0f }));
    }

    // we need to re-allocate the new mesh to the buffer (in case we have changed the curve)
    Renderer::Start();
}*/

// the float value can be 0.0f to the number of "points - 1" (example: 4.0f, and 5 points; we would end up on the fifth point)
void cs::BSpline::Interpolate(const float& value, Vector3& point)
{
    size_t _roundVal{ (size_t)value };
    Point3D* _p0{ &points[_roundVal] };
    Point3D* _p1{ &points[_roundVal + 1] };
    float _rest{ value - _roundVal }, _in{ 1.0f - _rest };

    Vector4 _outpoint{ *matrixWarp * Vector4(_in * _in * _in * _p0->position +
        _in * _in * _rest * (_p0->tangentOut + _p0->position) * 3.0f +
        _in * _rest * _rest * (_p1->tangentIn + _p1->position) * 3.0f +
        _rest * _rest * _rest * _p1->position, 1.0f) };

    point.x = _outpoint.x;
    point.y = _outpoint.y;
    point.z = _outpoint.z;
}

void cs::BSpline::InterpolateNoMatrix(const float& value, Vector3& point)
{
    size_t _roundVal{ (size_t)value };
    Point3D* _p0{ &points[_roundVal] };
    Point3D* _p1{ &points[_roundVal + 1] };
    float _rest{ value - _roundVal }, _in{ 1.0f - _rest };

    point = _in * _in * _in * _p0->position +
        _in * _in * _rest * (_p0->tangentOut + _p0->position) * 3.0f +
        _in * _rest * _rest * (_p1->tangentIn + _p1->position) * 3.0f +
        _rest * _rest * _rest * _p1->position;
}
