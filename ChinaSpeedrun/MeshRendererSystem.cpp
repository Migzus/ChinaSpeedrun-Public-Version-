#include "MeshRendererSystem.h"

#include "MeshRenderer.h"
#include "TransformComponent.h"
#include "ChinaEngine.h"
#include "Mathf.h"

void MeshRendererSystem::UpdatePosition(cs::MeshRenderer& mr, TransformComponent& tc)
{
	tc.position.x += 0.01f;
	tc.transform = glm::translate(Matrix4x4(1.f), tc.position);
	mr.ubo->model = tc.transform;
}
