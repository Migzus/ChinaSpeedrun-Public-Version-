#pragma once

namespace cs
{
	class Mesh;
	class GameObject;

	namespace algorithm
	{
		class MarchingTriangles
		{
		public:
			static void Polygonize(GameObject* go, const Mesh* mesh, const float& heightDifference = 1.0f);
		};
	}
}
