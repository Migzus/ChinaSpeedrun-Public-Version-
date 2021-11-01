#pragma once

#include "Mathf.h"
#include "Component.h"

namespace cs
{
	class TransformComponent : public Component
	{
	public:
		friend class Transform;

		// editor variable only
		OBB obb;

		Vector3 position{ Vector3(0.0f, 0.0f, 0.0f) }, rotation{ Vector3(0.0f, 0.0f, 0.0f) }, rotationDegrees{ Vector3(0.0f, 0.0f, 0.0f) }, scale{ Vector3(1.0f, 1.0f, 1.0f) };
		Vector3 globalPosition{ Vector3(0.0f, 0.0f, 0.0f) }, globalRotation{ Vector3(0.0f, 0.0f, 0.0f) }, globalScale{ Vector3(1.0f, 1.0f, 1.0f) };

		operator Matrix4x4&();
		virtual void ImGuiDrawComponent() override;

	protected:
		Matrix4x4 matrix{ Matrix4x4(1.0f) };
	};

	class Transform
	{
	public:
		static void DrawComponent();
		static void CalculateMatrix(TransformComponent& transform);
		static Matrix4x4& GetMatrixTransform(TransformComponent& transform);
	};
}
