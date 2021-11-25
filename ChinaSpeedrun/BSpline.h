#pragma once

#include "Mathf.h"
#include "Component.h"

#include <vector>

namespace cs
{
	class BSpline : public Component
	{
	public:
        struct Point3D
        {
            Vector3 position, tangentIn, tangentOut;

            Point3D();
            Point3D(const Vector3& position, const Vector3& tangent);
            Point3D(const Vector3& position, const Vector3& tangentIn, const Vector3& tangentOut);
        };

        std::vector<Point3D> points;
        size_t subdivisions;

        BSpline();
        BSpline(std::vector<Point3D> points);
        BSpline(std::vector<Point3D> points, size_t subDivs);

        virtual void Init() override;
        virtual void ImGuiDrawComponent() override;

        void MakeDrawLine();
        void Interpolate(const float& value, Vector3& point);
        void InterpolateNoMatrix(const float& value, Vector3& point);

	private:
        //struct DrawItem item;

        Matrix4x4* matrixWarp;
        int size;
	};
}
