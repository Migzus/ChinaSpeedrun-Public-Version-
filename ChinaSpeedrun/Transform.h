#pragma once

#include "Mathf.h"
#include "Component.h"

namespace cs
{
	class TransformComponent : public Component
	{
	public:
		friend class Transform;

		Vector3 position{ Vector3(0.0f, 0.0f, 0.0f) }, rotation{ Vector3(0.0f, 0.0f, 0.0f) }, rotationDegrees{ Vector3(0.0f, 0.0f, 0.0f) }, scale{ Vector3(1.0f, 1.0f, 1.0f) };
		Vector3 globalPosition{ Vector3(0.0f, 0.0f, 0.0f) }, globalRotation{ Vector3(0.0f, 0.0f, 0.0f) }, globalScale{ Vector3(1.0f, 1.0f, 1.0f) };

		TransformComponent();

		operator Matrix4x4&();
		TransformComponent& operator+=(const TransformComponent& transform);

		void Translate(const Vector3& translation);
		void Rotate(const Vector3& rotate);
		void Scale(const Vector3& scale);

		void SetPosition(const Vector3& pos);
		void SetRotation(const Vector3& rot);
		void SetRotationInDegrees(const Vector3& rotDeg);
		void SetScale(const Vector3& scale);

		virtual void Init() override;
		virtual void ImGuiDrawComponent() override;
		void GenerateOBBExtents(OBB& obb);
		
		void RootTraverseChildren();

	protected:
		Matrix4x4 matrix{ Matrix4x4(1.0f) };

	private:
		bool updateTransform;

		void UpdateGlobalTransform();
		void TraverseChildren(TransformComponent& transform, bool& dirty);
	};

	class Transform
	{
	public:
		static void CalculateMatrix(TransformComponent& transform);
		static void CalculateMatrix(TransformComponent& transform, Matrix4x4& matrixOffset);
		static Matrix4x4& GetMatrixTransform(TransformComponent& transform);
	};
}
