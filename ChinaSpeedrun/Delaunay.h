#pragma once

#include "Mathf.h"

#include <vector>

namespace cs
{
	class Mesh;

	namespace algorithm
	{
		class Delaunay
		{
		public:
			static Mesh* Triangulate(std::vector<Vector3>& points);

		private:
			struct Edge
			{
				uint32_t a, b;

				Edge(const uint32_t& na, const uint32_t& nb);

				bool operator==(const Edge& other);
			};

			static float ContainsInCircumcircle3D(const Vector3& point, const Vector3& aVec, const Vector3& bVec, const Vector3& cVec);
			static float ContainsInCircumcircle2D(const Vector3& point, const Vector3& aVec, const Vector3& bVec, const Vector3& cVec);
			static std::vector<Vector3> GenerateSuperTriangle(const std::vector<Vector3>& points);

			static void PushUnique(std::vector<uint32_t>& arrayRef, const uint32_t& value);
		};
	}
}
