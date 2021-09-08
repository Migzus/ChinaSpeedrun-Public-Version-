#include "Mesh.h"

#include <iterator>

Mesh::Mesh() :
	vertices{}, indices{}
{}

Mesh::Mesh(std::vector<Vertex> vertexArray, std::vector<uint16_t> indexArray) :
	vertices{vertexArray}, indices{indexArray}
{}

Mesh* Mesh::CreateDefaultPlane(const Vector2 extent)
{
	const std::vector<Vertex> _vertices
	{
		{{-extent.x, -extent.y, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{extent.x, -extent.y, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
		{{extent.x, extent.y, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
		{{-extent.x, extent.y, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
	};

	const std::vector<uint16_t> _indices
	{
		0, 1, 2, 2, 3, 0
	};

	return new Mesh(_vertices, _indices);
}

Mesh* Mesh::CreateDefaultCube(const Vector3 extent)
{
	const std::vector<Vertex> _vertices
	{
		{{-extent.x, -extent.y, extent.z}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{extent.x, -extent.y, extent.z}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
		{{extent.x, extent.y, extent.z}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
		{{-extent.x, extent.y, extent.z}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
		{{-extent.x, -extent.y, -extent.z}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{extent.x, -extent.y, -extent.z}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
		{{extent.x, extent.y, -extent.z}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
		{{-extent.x, extent.y, -extent.z}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
	};

	const std::vector<uint16_t> _indices
	{
		0, 1, 2, 2, 3, 0,
		4, 0, 7, 7, 0, 3,
		2, 7, 3, 2, 6, 7,
		2, 1, 6, 6, 1, 5,
		1, 0, 4, 4, 5, 1,
		5, 4, 6, 6, 4, 7
	};

	return new Mesh(_vertices, _indices);
}

Mesh* Mesh::CreateDefaultIcoSphere(const uint8_t subdivisions, const float radius)
{
	std::vector<Vertex> _vertices{};
	std::vector<uint16_t> _indices{};

	return new Mesh(_vertices, _indices);
}

void Mesh::SetMesh(std::vector<Vertex> vertexArray, std::vector<uint16_t> indexArray)
{
	vertices = vertexArray;
	indices = indexArray;
}

std::vector<Vertex> const& Mesh::GetVertices() const
{
	return vertices;
}

std::vector<uint16_t> const& Mesh::GetIndices() const
{
	return indices;
}
