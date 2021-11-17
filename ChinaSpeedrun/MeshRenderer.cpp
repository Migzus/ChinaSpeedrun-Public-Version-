#include "MeshRenderer.h"

#include "VulkanEngineRenderer.h"
#include "Transform.h"
#include "Mesh.h"
#include "ChinaEngine.h"
#include "CameraComponent.h"
#include "Camera.h"
#include "Material.h"
#include "Shader.h"
#include "GameObject.h"

#include "SceneManager.h"
#include "Scene.h"

void cs::MeshRenderer::UpdateUBO(MeshRendererComponent& meshRenderer, TransformComponent& transform)
{
	meshRenderer.ubo.model = transform;
}

void cs::MeshRenderer::UpdateUBO(MeshRendererComponent& meshRenderer, TransformComponent& transform, CameraBase& camera)
{
	meshRenderer.ubo.model = transform;
	meshRenderer.ubo.proj = Camera::GetProjectionMatrix(camera);
	meshRenderer.ubo.view = Camera::GetViewMatrix(camera);
}

cs::MeshRendererComponent::MeshRendererComponent() :
	mesh{ nullptr }
{
	ChinaEngine::renderer.SolveRenderer(this, Solve::ADD);
	SceneManager::GetCurrentScene()->AddToRenderQueue(this);
	uboOffset = SceneManager::GetCurrentScene()->GetUBOOffset();
}

void cs::MeshRendererComponent::SetMesh(Mesh* mesh)
{
	this->mesh = mesh;
	GenerateOBBExtents(gameObject->obb);
}

void cs::MeshRendererComponent::Init()
{
	
}

void cs::MeshRendererComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool _visible{ visible };
		ImGui::Checkbox("Visible", &_visible);
		if (mesh != nullptr)
			ImGui::Text("Mesh: %s", mesh->resourcePath);
		SetVisible(_visible);

		ImGui::TreePop();
	}
}

bool cs::MeshRendererComponent::IsRendererValid() const
{
	return RenderComponent::IsRendererValid() && mesh != nullptr && material != nullptr;
}

void cs::MeshRendererComponent::VulkanDraw(VkCommandBuffer& commandBuffer, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline);

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &mesh->vertexBufferOffset);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, mesh->indexBufferOffset, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->shader->layout, 0, 1, &descriptorSets[index], 0, nullptr);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh->GetIndices().size()), 1, 0, 0, 0);
}

void cs::MeshRendererComponent::GenerateOBBExtents(OBB& obb)
{
	if (mesh == nullptr || mesh->GetVertices().empty())
	{
		Debug::LogWarning("Cannot generate OBB from mesh renderer. Either it has no mesh, or the mesh data is empty.");
		return;
	}

	Vector3 _maxExtent{ Vector3(0.0f) }, _minExtent{ Vector3(0.0f) };
	for (auto& vertex : mesh->GetVertices())
	{
		_maxExtent.x = Mathf::Max(_maxExtent.x, vertex.position.x);
		_maxExtent.y = Mathf::Max(_maxExtent.y, vertex.position.y);
		_maxExtent.z = Mathf::Max(_maxExtent.z, vertex.position.z);

		_minExtent.x = Mathf::Min(_minExtent.x, vertex.position.x);
		_minExtent.y = Mathf::Min(_minExtent.y, vertex.position.y);
		_minExtent.z = Mathf::Min(_minExtent.z, vertex.position.z);
	}

	obb = { _minExtent, _maxExtent };
}

cs::MeshRendererComponent::~MeshRendererComponent()
{
	ChinaEngine::renderer.SolveRenderer(this, Solve::REMOVE, true);
}
