#include "Draw.h"

#include "Color.h"
#include "ChinaEngine.h"
#include "VulkanEngineRenderer.h"
#include "ResourceManager.h"
#include "RenderComponent.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Camera.h"

#include "Scene.h"
#include "SceneManager.h"

cs::Material* cs::Draw::material{ nullptr };
bool cs::Draw::updateVertexIndexBuffers{ false };
std::vector<cs::DrawItem*> cs::Draw::debugItems;
VkDescriptorPool cs::Draw::descriptorPool;
std::vector<VkDescriptorSet> cs::Draw::descriptorSets;
cs::UniformBufferObject cs::Draw::ubo;

void cs::Draw::Setup()
{
	Shader* _shader{ ResourceManager::Load<Shader>("../Resources/shaders/line_shader") };
	_shader->AssignShaderVertexInputAttrib("position", 0, Shader::Data::VEC3);
	_shader->AssignShaderVertexInputAttrib("color", 1, Shader::Data::VEC3);
	_shader->AssignShaderVertexInputAttrib("uv", 2, Shader::Data::VEC2);
	_shader->AssignShaderVertexBinding(Shader::InputRate::VERTEX);

	_shader->AssignShaderDescriptor("ubo", 0, Shader::Type::VERTEX, Shader::Data::UNIFORM);

	Material* _material{ ResourceManager::Load<Material>("../Resources/materials/line_material.mat") };
	//_material->renderMode = Material::RenderMode::TRANSPARENT_;
	_material->lineWidth = 1.0f;
	_material->fillMode = Material::FillMode::LINE;
	_material->cullMode = Material::CullMode::NONE;
	_material->shader = _shader;

	material = _material;
}

void cs::Draw::Update()
{
	ubo.model = Matrix4x4(1.0f);
	ubo.proj = Camera::GetProjectionMatrix(*SceneManager::mainCamera);
	ubo.view = Camera::GetViewMatrix(*SceneManager::mainCamera);
}

auto cs::Draw::MakeLine(const Vector3& startPos, const Vector3& endPos)
{
	return std::make_tuple(startPos, endPos, endPos);
}

cs::DrawItem* cs::Draw::Line(const std::vector<Vector3>& points, std::vector<Color> colors, const DrawMode& drawMode, Scene* sceneToAttatchTo)
{
	return Line(points, colors, drawMode, descriptorSets, sceneToAttatchTo);
}

cs::DrawItem* cs::Draw::Line(const std::vector<Vector3>& points, std::vector<Color> colors, const DrawMode& drawMode, std::vector<VkDescriptorSet>& descriptorSetsRef, Scene* sceneToAttatchTo)
{
	if (points.size() <= 1)
		return nullptr;

	if (colors.empty())
		colors.push_back(Color::white);

	std::vector<Vertex> _vertcies;
	std::vector<uint32_t> _indcies;

	const size_t _increase{ static_cast<size_t>(drawMode == DrawMode::SOLID ? 1 : 2) };
	const size_t _size{ points.size() - (drawMode == DrawMode::SOLID ? 1 : 3) };

	for (size_t i{ 0 }; i < _size; i += _increase)
	{
		Color _finalColor{ colors[i * (colors.size() / points.size())] };
		auto [_start, _mid, _end]{ MakeLine(points[i], points[i + 1]) };

		_indcies.push_back(static_cast<uint32_t>(_vertcies.size()));
		_vertcies.push_back({ _start, _finalColor.ConvertToGLMVector(), Vector2(0.0f) });
		_indcies.push_back(static_cast<uint32_t>(_vertcies.size()));
		_vertcies.push_back({ _mid, _finalColor.ConvertToGLMVector(), Vector2(0.0f) });
		_indcies.push_back(static_cast<uint32_t>(_vertcies.size()));
		_vertcies.push_back({ _end, _finalColor.ConvertToGLMVector(), Vector2(0.0f) });
	}

	Mesh* _mesh{ new Mesh };
	_mesh->SetMesh(_vertcies, _indcies);
	_mesh->resourcePath = "debug_element_" + std::to_string(debugItems.size());
	
	DrawItem* _newItem{ new DrawItem };
	_newItem->mesh = _mesh;
	_newItem->SetDescriptorRefs(descriptorSetsRef);
	if (sceneToAttatchTo == nullptr)
		debugItems.push_back(_newItem);
	else
		sceneToAttatchTo->PushDebugItem(_newItem);

	updateVertexIndexBuffers = true;

	return _newItem;
}

void cs::Draw::MeshLine(Mesh* mesh, Scene* sceneToAttatchTo)
{
	DrawItem* _newItem{ new DrawItem };
	_newItem->mesh = mesh;
	_newItem->SetDescriptorRefs(descriptorSets);

	if (sceneToAttatchTo == nullptr)
		debugItems.push_back(_newItem);
	else
		sceneToAttatchTo->PushDebugItem(_newItem);

	updateVertexIndexBuffers = true;
}

void cs::Draw::Rectangle(const Vector3& extents, const Vector3& position, Scene* sceneToAttatchTo)
{
	if (Mathf::IsVectorZero(extents))
		return;

	updateVertexIndexBuffers = true;
}

void cs::Draw::Circle(const float& radius, const Vector3& position, const Color& color, const uint32_t& resolution, Scene* sceneToAttatchTo)
{
	if (radius <= 0.0f)
		return;

	updateVertexIndexBuffers = true;
}

void cs::Draw::Image(const Texture* texture, const Vector3& position, const Vector3& size, Scene* sceneToAttatchTo)
{
	if (texture == nullptr || Mathf::IsVectorZero(size))
		return;

	updateVertexIndexBuffers = true;
}

void cs::Draw::VulkanDraw(VkCommandBuffer& commandBuffer, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline);

	for (auto* item : debugItems)
	{
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &item->mesh->vertexBufferOffset);
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer, item->mesh->indexBufferOffset, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->shader->layout, 0, 1, item->descriptorSetsRef[index], 0, nullptr);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(item->mesh->GetIndices().size()), 1, 0, 0, 0);
	}

	if (SceneManager::HasScenes())
		return;

	for (auto* item : SceneManager::GetCurrentScene()->GetDrawItems())
	{
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &item->mesh->vertexBufferOffset);
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer, item->mesh->indexBufferOffset, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->shader->layout, 0, 1, item->descriptorSetsRef[index], 0, nullptr);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(item->mesh->GetIndices().size()), 1, 0, 0, 0);
	}
}

void cs::Draw::CreateDescriptorSets()
{
	ChinaEngine::renderer.CreateDebugDrawDescriptorPool();
	ChinaEngine::renderer.CreateDebugDrawDescriptorSets();
}

void cs::Draw::DebugGrid()
{
	Mesh* _gridMesh{ new Mesh };
	std::vector<Vertex> _vertcies{};
	std::vector<uint32_t> _indcies{};
	const Vector3 _extents{ Vector3(100.0f) };

	for (float x{ -_extents.x }; x < _extents.x + 1.0f; x++)
	{
		const Vector3 _color{ (int)x == 0 ? Vector3(0.0f, 0.0f, 1.0f) : (int)x % 10 == 0 ? Vector3(0.1f) : Vector3(0.01f) };

		_indcies.push_back(static_cast<uint32_t>(_vertcies.size()));
		_vertcies.push_back({ Vector3(x, 0.0f, _extents.z), _color, Vector2(0.0f) });
		_indcies.push_back(static_cast<uint32_t>(_vertcies.size()));
		_vertcies.push_back({ Vector3(x, 0.0f, -_extents.z), _color, Vector2(0.0f) });
		_indcies.push_back(static_cast<uint32_t>(_vertcies.size()));
		_vertcies.push_back({ Vector3(x, 0.0f, 0.0f), _color, Vector2(0.0f) });
	}

	for (float z{ -_extents.z }; z < _extents.z + 1.0f; z++)
	{
		const Vector3 _color{ (int)z == 0 ? Vector3(1.0f, 0.0f, 0.0f) : (int)z % 10 == 0 ? Vector3(0.1f) : Vector3(0.01f) };

		_indcies.push_back(static_cast<uint32_t>(_vertcies.size()));
		_vertcies.push_back({ Vector3(_extents.x, 0.0f, z), _color, Vector2(0.0f) });
		_indcies.push_back(static_cast<uint32_t>(_vertcies.size()));
		_vertcies.push_back({ Vector3(-_extents.x, 0.0f, z), _color, Vector2(0.0f) });
		_indcies.push_back(static_cast<uint32_t>(_vertcies.size()));
		_vertcies.push_back({ Vector3(0.0f, 0.0f, z), _color, Vector2(0.0f) });
	}

	_indcies.push_back(static_cast<uint32_t>(_vertcies.size()));
	_vertcies.push_back({ Vector3(0.0f, _extents.y, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f) });
	_indcies.push_back(static_cast<uint32_t>(_vertcies.size()));
	_vertcies.push_back({ Vector3(0.0f, -_extents.y, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f) });
	_indcies.push_back(static_cast<uint32_t>(_vertcies.size()));
	_vertcies.push_back({ Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f) });

	_gridMesh->SetMesh(_vertcies, _indcies);
	_gridMesh->resourcePath = "grid";

	DrawItem* _item{ new DrawItem };
	_item->mesh = _gridMesh;
	_item->SetDescriptorRefs(descriptorSets);

	debugItems.push_back(_item);
}

cs::DrawItem::DrawItem() :
	mesh{ nullptr }
{}

void cs::DrawItem::UpdateMesh()
{
	ChinaEngine::renderer.SolveMesh(mesh, Solve::UPDATE);
}

void cs::DrawItem::SetDescriptorRefs(std::vector<VkDescriptorSet>& sets)
{
	for (auto& descriptorSet : sets)
		descriptorSetsRef.push_back(&descriptorSet);
}
