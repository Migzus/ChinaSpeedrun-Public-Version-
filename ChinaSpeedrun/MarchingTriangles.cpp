#include "MarchingTriangles.h"

#include "Mesh.h"
#include "GameObject.h"
#include "BSpline.h"
#include "Draw.h"

void cs::algorithm::MarchingTriangles::Polygonize(GameObject* go, const Mesh* mesh, const float& heightDifference)
{
	float _min{ FLT_MAX }, _max{ -FLT_MAX };

	for (auto& vertex : mesh->GetVertices())
	{
		_min = Mathf::Min(_min, vertex.position.y);
		_max = Mathf::Max (_max, vertex.position.y);
	}
	
	const float _difference{ (_max - _min) / heightDifference };

	for (float level{ _min }; level < _max; level += _difference)
		Draw::Line(MakeHeightCurve(mesh, level), { Color::lime }, Draw::DrawMode::SOLID);
}

std::vector<Vector3> cs::algorithm::MarchingTriangles::MakeHeightCurve(const Mesh* mesh, const float& heightLevel)
{


	return std::vector<Vector3>();
}
