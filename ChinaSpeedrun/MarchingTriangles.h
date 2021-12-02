#pragma once

#include "Mathf.h"

#include <vector>

namespace cs
{
	struct DrawItem;
	class Mesh;
	class GameObject;

	namespace algorithm
	{
		class MarchingTriangles
		{
		public:
			static void Polygonize(GameObject* go, const Mesh* mesh, const float& heightDifference = 1.0f);

		private:
			static std::vector<Vector3> MakeHeightCurve(const Mesh* mesh , const float& heightLevel);
		};
	}
}
