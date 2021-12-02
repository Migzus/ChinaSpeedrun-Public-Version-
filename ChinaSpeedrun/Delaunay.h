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
			enum class AxisMode
			{
				X,
				Y,
				Z
			};

			static Mesh* Triangulate(std::vector<Vector3>& points, const bool& invertFaces = false, const AxisMode& mode = AxisMode::Y);

		private:
			struct Edge
			{
				uint32_t a, b;

				Edge(const uint32_t& na, const uint32_t& nb);

				bool operator==(const Edge& other);
			};

			static float ContainsInCircumcircle3D(const Vector3& point, const Vector3& aVec, const Vector3& bVec, const Vector3& cVec, const AxisMode& mode);
			static float ContainsInCircumcircle2D(const Vector3& point, const Vector3& aVec, const Vector3& bVec, const Vector3& cVec, const AxisMode& mode);
			static std::vector<Vector3> GenerateSuperTriangle(const std::vector<Vector3>& points);

			static void PushUnique(std::vector<uint32_t>& arrayRef, const uint32_t& value);
		};
	}
}
