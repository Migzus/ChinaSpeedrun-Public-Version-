#include "Delaunay.h"

#include "Mesh.h"

#include <algorithm>

cs::Mesh* cs::algorithm::Delaunay::Triangulate(std::vector<Vector3>& points)
{
	if (points.size() < 3)
		return nullptr;

	std::vector<Vector3> _superTriangle{ GenerateSuperTriangle(points) };
	points.insert(points.begin(), _superTriangle.begin(), _superTriangle.end());
	std::vector<uint32_t> _indcies{ 0, 1, 2 }; // the super triangle's indcies

	// go trough each point
	for (size_t j{ 3 }; j < points.size(); j++)
	{
		std::vector<Edge> _edgeBuffer;

		// go trough each triangle
		for (size_t i{ 0 }; i < _indcies.size(); i += 3)
		{
			const uint32_t _indexA{ _indcies[i] };
			const uint32_t _indexB{ _indcies[i + 1] };
			const uint32_t _indexC{ _indcies[i + 2] };

			if (ContainsInCircumcircle2D(points[j], points[_indexA], points[_indexB], points[_indexC]) > 0.0f)
			{
				_edgeBuffer.push_back(Edge(_indexA, _indexB));
				_edgeBuffer.push_back(Edge(_indexB, _indexC));
				_edgeBuffer.push_back(Edge(_indexC, _indexA));
				
				// remove the current triangle
				_indcies.erase(_indcies.begin() + i, _indcies.begin() + i + 3);
			}
		}

		for (size_t a{ 0 }; a < _edgeBuffer.size(); a++)
		{
			for (size_t b{ 0 }; b < a; b++)
			{
				if (_edgeBuffer.size() > a && _edgeBuffer[a] == _edgeBuffer[b])
				{
					_edgeBuffer.erase(_edgeBuffer.begin() + a);
					a--;
					_edgeBuffer.erase(_edgeBuffer.begin() + b);
					b--;
					continue;
				}
			}
		}

		for (size_t i{ 0 }; i < _edgeBuffer.size(); i++)
		{
			_indcies.push_back(j);
			_indcies.push_back(_edgeBuffer[i].a);
			_indcies.push_back(_edgeBuffer[i].b);
		}
	}

	/*std::vector<size_t> _removeTriangleIndcies;

	// find the connections to the super triangle
	for (size_t i{ 0 }; i < _indcies.size(); i += 3)
	{
		// are any of the indcies connected to the super triangle?
		// aka. 0, 1 or 2?
		if (_indcies[i] == 0 || _indcies[i + 1] == 0 || _indcies[i + 2] == 0 ||
			_indcies[i] == 1 || _indcies[i + 1] == 1 || _indcies[i + 2] == 1 ||
			_indcies[i] == 2 || _indcies[i + 1] == 2 || _indcies[i + 2] == 2)
			_removeTriangleIndcies.push_back(i);
		else
		{
			_indcies[i] -= 3;
			_indcies[i + 1] -= 3;
			_indcies[i + 2] -= 3;
		}
	}

	// we reverse it because the index ordering will be messed up; deleting from the front
	std::reverse(_removeTriangleIndcies.begin(), _removeTriangleIndcies.end());

	// actually remove the indcies
	for (auto& index : _removeTriangleIndcies)
		_indcies.erase(_indcies.begin() + index, _indcies.begin() + index + 3);

	// Delete the super triangle points
	points.erase(points.begin(), points.begin() + 3);*/

	// ---- Construct Mesh -------------------------------------------------------------

	std::vector<Vertex> _vertcies;

	for (auto& point : points)
		_vertcies.push_back({ point, Vector3(1.0f), Vector2(0.0f) });

    return new Mesh(_vertcies, _indcies);
}

std::vector<Vector3> cs::algorithm::Delaunay::GenerateSuperTriangle(const std::vector<Vector3>& points)
{
	float _magnitude{ points[0].x };

	for (uint32_t i{ 1 }; i < points.size(); i++)
	{
		const float _aX{ abs(points[i].x) };
		const float _aY{ abs(points[i].y) };
		const float _aZ{ abs(points[i].z) };

		if (_aX > _magnitude)
			_magnitude = _aX;
		if (_aY > _magnitude)
			_magnitude = _aY;
		if (_aZ > _magnitude)
			_magnitude = _aZ;
	}

	return { Vector3(_magnitude * 10.0f, 0.0f, 0.0f), Vector3(0.0f, _magnitude * 10.0f, 0.0f), Vector3(0.0f, 0.0f, _magnitude * 10.0f) };
}

void cs::algorithm::Delaunay::PushUnique(std::vector<uint32_t>& arrayRef, const uint32_t& value)
{
	for (auto& arrayVal : arrayRef)
		if (arrayVal == value)
			return;

	arrayRef.push_back(value);
}

float cs::algorithm::Delaunay::ContainsInCircumcircle3D(const Vector3& point, const Vector3& aVec, const Vector3& bVec, const Vector3& cVec)
{
	const Vector3 _vecDeltaA{ aVec - point };
	const Vector3 _vecDeltaB{ bVec - point };
	const Vector3 _vecDeltaC{ cVec - point };
	const Vector3 _vecCrossAB{ Mathf::CrossProduct(_vecDeltaA, _vecDeltaB) };
	const Vector3 _vecCrossBC{ Mathf::CrossProduct(_vecDeltaB, _vecDeltaC) };
	const Vector3 _vecCrossCA{ Mathf::CrossProduct(_vecDeltaC, _vecDeltaA) };
	const Vector3 _vecSquaredA{ glm::pow(_vecDeltaA, Vector3(2.0f)) };
	const Vector3 _vecSquaredB{ glm::pow(_vecDeltaB, Vector3(2.0f)) };
	const Vector3 _vecSquaredC{ glm::pow(_vecDeltaC, Vector3(2.0f)) };

	return Mathf::DotProduct(_vecCrossBC, _vecSquaredA) + Mathf::DotProduct(_vecCrossCA, _vecSquaredB) + Mathf::DotProduct(_vecCrossAB, _vecSquaredC);
}

float cs::algorithm::Delaunay::ContainsInCircumcircle2D(const Vector3& point, const Vector3& aVec, const Vector3& bVec, const Vector3& cVec)
{
	const Vector2 _vecDeltaA{ Vector2(aVec.x, aVec.y) - Vector2(point.x, point.y) };
	const Vector2 _vecDeltaB{ Vector2(bVec.x, bVec.y) - Vector2(point.x, point.y) };
	const Vector2 _vecDeltaC{ Vector2(cVec.x, cVec.y) - Vector2(point.x, point.y) };
	const float _crossAB{ Mathf::CrossProduct(_vecDeltaA, _vecDeltaB) };
	const float _crossBC{ Mathf::CrossProduct(_vecDeltaB, _vecDeltaC) };
	const float _crossCA{ Mathf::CrossProduct(_vecDeltaC, _vecDeltaA) };
	const float _squaredA{ _vecDeltaA.x * _vecDeltaA.x + _vecDeltaA.y * _vecDeltaA.y };
	const float _squaredB{ _vecDeltaB.x * _vecDeltaB.x + _vecDeltaB.y * _vecDeltaB.y };
	const float _squaredC{ _vecDeltaC.x * _vecDeltaC.x + _vecDeltaC.y * _vecDeltaC.y };

	return _crossBC * _squaredA + _crossCA * _squaredB + _crossAB * _squaredC;
}

cs::algorithm::Delaunay::Edge::Edge(const uint32_t& na, const uint32_t& nb) :
	a{ na }, b{ nb }
{}

bool cs::algorithm::Delaunay::Edge::operator==(const Edge& other)
{
	return (a == other.a && b == other.b) || (a == other.b && b == other.a);
}
