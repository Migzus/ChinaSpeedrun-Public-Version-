#include "Delaunay.h"

#include "Mesh.h"

#include <algorithm>

cs::Mesh* cs::algorithm::Delaunay::Triangulate(std::vector<Vector3>& points, const bool& invertFaces, const AxisMode& mode)
{
	if (points.size() < 3)
		return nullptr;

	std::vector<Vector3> _superTriangle{ GenerateSuperTriangle(points) };
	points.insert(points.begin(), _superTriangle.begin(), _superTriangle.end());
	std::vector<uint32_t> _indcies{ 0, 1, 2 }; // the super triangle's indcies

	// go trough each point
	for (size_t j{ 2 }; j < points.size(); j++)
	{
		std::vector<Edge> _edgeBuffer;

		// go trough each triangle
		for (int i{ (int)_indcies.size() - 3 }; i >= 0; i -= 3)
		{
			const uint32_t _indexA{ _indcies[i] };
			const uint32_t _indexB{ _indcies[i + 1] };
			const uint32_t _indexC{ _indcies[i + 2] };

			if (ContainsInCircumcircle2D(points[j], points[_indexA], points[_indexB], points[_indexC], mode) > 0.0f)
			{
				_edgeBuffer.push_back(Edge(_indexA, _indexB));
				_edgeBuffer.push_back(Edge(_indexB, _indexC));
				_edgeBuffer.push_back(Edge(_indexC, _indexA));
				
				// remove the current triangle
				_indcies.erase(_indcies.begin() + i, _indcies.begin() + i + 3);
			}
		}

		for (int a{ (int)_edgeBuffer.size() - 2 }; a >= 0; a--)
		{
			for (int b{ (int)_edgeBuffer.size() - 1 }; b >= a + 1; b--)
			{
				if (_edgeBuffer[a] == _edgeBuffer[b])
				{
					_edgeBuffer.erase(_edgeBuffer.begin() + b);
					_edgeBuffer.erase(_edgeBuffer.begin() + a);
					b--;
					continue;
				}
			}
		}
		
		for (size_t i{ 0 }; i < _edgeBuffer.size(); i++)
		{
			_indcies.push_back(_edgeBuffer[i].a);
			_indcies.push_back(_edgeBuffer[i].b);
			_indcies.push_back(j);
		}
	}

	std::vector<size_t> _removeTriangleIndcies;

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

	// we reverse it because the index ordering will be messed up, if deleting from the front
	std::reverse(_removeTriangleIndcies.begin(), _removeTriangleIndcies.end());

	// actually remove the indcies
	for (auto& index : _removeTriangleIndcies)
		_indcies.erase(_indcies.begin() + index, _indcies.begin() + index + 3);

	// Delete the super triangle points
	points.erase(points.begin(), points.begin() + 3);

	// invert the faces if we ask for it
	if (invertFaces)
		for (size_t i{ 0 }; i < _indcies.size(); i += 3)
			std::swap(_indcies[i], _indcies[i + 1]);

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

	return {
		Vector3(_magnitude * 10.0f, 0.0f, 0.0f),
		Vector3(0.0f, _magnitude * 10.0f, 0.0f),
		Vector3(-_magnitude * 10.0f, -_magnitude * 10.0f, 0.0f) };
}

void cs::algorithm::Delaunay::PushUnique(std::vector<uint32_t>& arrayRef, const uint32_t& value)
{
	for (auto& arrayVal : arrayRef)
		if (arrayVal == value)
			return;

	arrayRef.push_back(value);
}

float cs::algorithm::Delaunay::ContainsInCircumcircle3D(const Vector3& point, const Vector3& aVec, const Vector3& bVec, const Vector3& cVec, const AxisMode& mode)
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

float cs::algorithm::Delaunay::ContainsInCircumcircle2D(const Vector3& point, const Vector3& aVec, const Vector3& bVec, const Vector3& cVec, const AxisMode& mode)
{
	Vector2 _offset{ 0.0f };
	Vector2 _vecA{ 0.0f };
	Vector2 _vecB{ 0.0f };
	Vector2 _vecC{ 0.0f };

	switch (mode)
	{
	case AxisMode::X:
		_offset = Vector2(point.y, point.z);
		_vecA = Vector2(aVec.y, aVec.z);
		_vecB = Vector2(bVec.y, bVec.z);
		_vecC = Vector2(cVec.y, cVec.z);
		break;
	case AxisMode::Y:
		_offset = Vector2(point.x, point.z);
		_vecA = Vector2(aVec.x, aVec.z);
		_vecB = Vector2(bVec.x, bVec.z);
		_vecC = Vector2(cVec.x, cVec.z);
		break;
	case AxisMode::Z:
		_offset = Vector2(point.x, point.y);
		_vecA = Vector2(aVec.x, aVec.y);
		_vecB = Vector2(bVec.x, bVec.y);
		_vecC = Vector2(cVec.x, cVec.y);
		break;
	}

	const Vector2 _vecDeltaA{ _vecA - _offset };
	const Vector2 _vecDeltaB{ _vecB - _offset };
	const Vector2 _vecDeltaC{ _vecC - _offset };
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
