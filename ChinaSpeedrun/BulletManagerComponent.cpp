#include "BulletManagerComponent.h"

#include "ChinaEngine.h"
#include "SceneManager.h"
#include "Scene.h"

#include "imgui.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Time.h"
#include "Input.h"
#include "CameraBase.h"
#include "Camera.h"

#include "Debug.h"

cs::BulletManagerComponent::BulletManagerComponent() :
	body{ nullptr }, quadSprite{ Mesh::CreateDefaultPlane(Vector2(0.5f)) }, ubos{ nullptr }, bulletShaderParams{ nullptr },
	bufferInfo{ VulkanBufferInfo() }, bulletCapacity{ 0 }, mainTex{ nullptr }, subTex{ nullptr }
{
	definition.type = b2_staticBody;

	ChinaEngine::renderer.SolveRenderer(this, Solve::ADD);
	SceneManager::GetCurrentScene()->AddToRenderQueue(this);
	uboOffset = SceneManager::GetCurrentScene()->GetUBOOffset();

	BulletType* _newType{ new BulletType };

	_newType->damage = 10;
	_newType->texExtents = Vector2(1.0f);
	_newType->texOffset = Vector2(0.0f);

	types["circle"] = _newType;
}

void cs::BulletManagerComponent::Init()
{

}

void cs::BulletManagerComponent::ImGuiDrawComponent()
{
	if (ImGui::TreeNodeEx("Bullet Manager", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragInt("Capacity", &bulletCapacity, 0.25f, 0, 10000);

		ImGui::Text("Main Border");
		ImGui::DragFloat("Width", &mainBorder.width, 0.1f);
		ImGui::DragFloat("Height", &mainBorder.height, 0.1f);
		ImGui::DragFloat2("Offset", &mainBorder.offset[0], 0.1f);

		ImGui::Text("Absolute Border");
		ImGui::DragFloat("Width", &absoluteBorder.width, 0.1f);
		ImGui::DragFloat("Height", &absoluteBorder.height, 0.1f);
		ImGui::DragFloat2("Offset", &absoluteBorder.offset[0], 0.1f);

		//Mathf::Clamp(bulletCapacity, 0, 10000);

		ImGui::Text("Current Bullet Count: %i", activeBullets.size());

		ImGui::TreePop();
	}
}

void cs::BulletManagerComponent::VulkanDraw(VkCommandBuffer& commandBuffer, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer)
{
	VkBuffer _vertexBuffers[]{ vertexBuffer, bufferInfo.buffer };
	VkDeviceSize _offsets[]{ quadSprite->vertexBufferOffset, 0 };

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline);
	vkCmdBindVertexBuffers(commandBuffer, 0, 2, _vertexBuffers, _offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, quadSprite->indexBufferOffset, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->shader->layout, 0, 1, &descriptorSets[index], 0, nullptr);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(quadSprite->GetIndices().size()), activeBullets.size(), 0, 0, 0);
}

void cs::BulletManagerComponent::CreateSystem()
{
	for (size_t i{ 0 }; i < bulletCapacity; i++)
	{
		Bullet* _newBullet{ new Bullet(this) };
		readyBullets.push(_newBullet);
	}

	ubos = new UniformBufferObject[bulletCapacity];
	bulletShaderParams = new BulletShaderParams[bulletCapacity];

	CreateBulletBuffer();
}

void cs::BulletManagerComponent::CreateBorders(const float& width, const float& height, const float& margin, const Vector2& offset)
{
	mainBorder.width = width;
	mainBorder.height = height;
	mainBorder.offset = offset;

	absoluteBorder.width = width + margin * 2.0f;
	absoluteBorder.height = height + margin * 2.0f;
	absoluteBorder.offset = offset;
}

void cs::BulletManagerComponent::UpdateUBO(CameraBase& camera)
{
	ubo.proj = Camera::GetProjectionMatrix(camera);
	ubo.view = Camera::GetViewMatrix(camera);
}

void cs::Bullet::BorderCheck()
{
	const Vector2 _border{ manager->absoluteBorder.width * 0.5f, manager->absoluteBorder.height * 0.5f };
	const Vector2 _offset{ manager->absoluteBorder.offset };

	if (!(Mathf::Within(-_border.x + _offset.x, params.position.x, _border.x + _offset.x) && Mathf::Within(-_border.y + _offset.y, params.position.y, _border.y + _offset.y)))
		manager->DespawnBullet(this);
}

void cs::BulletManagerComponent::Update()
{
	if (Input::GetActionPressed("accept"))
	{
		BulletInfo _info{ BulletInfo(types["circle"]) };

		_info.subColor = Color(Mathf::RandRange(0.0f, 1.0f), Mathf::RandRange(0.0f, 1.0f), Mathf::RandRange(0.0f, 1.0f));
		_info.speed = 0.1f;
		//_info.position = Vector2(0.0f);
		_info.rotation = Mathf::RandRange(0.0f, Mathf::TAU);

		//SpawnBullet(_info);
		
		SpawnCircle(_info, 1000);
	}

	uint32_t _index{ 0 };
	for (auto* bullet : activeBullets)
	{
		bullet->Update(_index);
		bullet->BorderCheck();
		_index++;
	}

	UpdateUBO(*SceneManager::mainCamera);

	if (!activeBullets.empty())
		ChinaEngine::renderer.CopyDataToBuffer(bufferInfo.bufferMemory, bulletShaderParams, 0, bufferInfo.bufferSize);
}

void cs::BulletManagerComponent::SpawnCircle(BulletInfo& info, const uint16_t bulletCount, const float radius, const float spacing)
{
	const Vector2 _origin{ info.position };
	const float _rotationOffset{ Mathf::TAU / float(bulletCount) };
	const float _startRotation{ info.rotation };

	for (uint16_t i{ 0 }; i < bulletCount; i++)
	{
		const float _currentRotation{ _rotationOffset * i + _startRotation + spacing };

		info.position = _origin + Vector2(cos(_currentRotation), sin(_currentRotation)) * radius;
		info.rotation = _currentRotation;

		SpawnBullet(info);
	}
}

cs::Bullet* cs::BulletManagerComponent::SpawnBullet(const BulletInfo& info)
{
	if (readyBullets.empty())
	{
		//Debug::LogWarning("Reached max capacity for bullet manager. Capacity = ", bulletCapacity);
		return nullptr;
	}

	Bullet* _newBullet{ readyBullets.front() };

	readyBullets.pop();
	_newBullet->SetInfo(info);
	activeBullets.push_back(_newBullet);

	return _newBullet;
}

void cs::BulletManagerComponent::DespawnBullet(const uint64_t& index)
{
	Bullet* _deactivatedBullet{ activeBullets[index] };
	activeBullets.erase(activeBullets.begin() + index);
	readyBullets.push(_deactivatedBullet);
}

void cs::BulletManagerComponent::DespawnBullet(Bullet* bullet)
{
	std::vector<Bullet*>::iterator _it{ std::find(activeBullets.begin(), activeBullets.end(), bullet) };

	if (_it != activeBullets.end())
		DespawnBullet(std::distance(activeBullets.begin(), _it));
}

void cs::BulletManagerComponent::DestroyBuffer()
{
	ChinaEngine::renderer.DestroyBuffer(bufferInfo);
}

void cs::BulletManagerComponent::DestroySystem()
{
	delete ubos;
	delete bulletShaderParams;

	for (auto bullet : activeBullets)
		delete bullet;

	activeBullets.clear();

	for (size_t i{ 0 }; i < readyBullets.size(); i++)
	{
		delete readyBullets.front();
		readyBullets.pop();
	}

	DestroyBuffer();
}

cs::BulletManagerComponent::~BulletManagerComponent()
{
	DestroySystem();

	ChinaEngine::renderer.SolveRenderer(this, Solve::REMOVE, true);
}

void cs::BulletManagerComponent::CreateBulletBuffer()
{
	bufferInfo.bufferSize = static_cast<VkDeviceSize>(sizeof(BulletShaderParams) * bulletCapacity);
	ChinaEngine::renderer.CreateBuffer(bufferInfo.bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferInfo.buffer, bufferInfo.bufferMemory);
}

cs::Bullet::Bullet(BulletManagerComponent* manager) :
	manager{ manager }, turnRate{ 0.0f }, acceleration{ 0.0f }, speed{ 0.0f },
	spin{ 0.0f }, gravity{ Vector2(0.0f) }, currentSpin{ 0.0f }, velocity{ Vector2(0.0f) },
	ubo{ UniformBufferObject() }, deltaRotation{ 0.0f },
	rotateWithVelocity{ 0.0f }, rotationOffset{ 0.0f }, params{ BulletShaderParams() }
{}

void cs::Bullet::Update(const uint32_t& index)
{
	velocity += gravity * Time::deltaTime;
	speed += acceleration * Time::deltaTime;
	currentSpin += spin * Time::deltaTime;
	deltaRotation += turnRate * Time::deltaTime;

	Vector2 _finalVelocity{ velocity + Vector2(cos(deltaRotation), sin(deltaRotation)) };
	params.rotation = deltaRotation * rotateWithVelocity + rotationOffset;
	params.position += _finalVelocity * speed * Time::deltaTime;
	
	manager->bulletShaderParams[index] = params;
}

void cs::Bullet::SetInfo(const BulletInfo& info)
{
	deltaRotation = info.rotation;
	speed = info.speed;
	acceleration = info.acceleration;
	turnRate = info.turnRotation;
	gravity = info.gravity;
	rotateWithVelocity = static_cast<float>(info.rotateWithVelocity);

	params.position = info.position;
	params.mainColor = info.mainColor;
	params.subColor = info.subColor;
}

cs::BulletInfo::BulletInfo(BulletType* type) :
	type{ type }, rotation{ 0.0f }, speed{ 0.0f }, position{ Vector2(0.0f) },
	rotationOffset{ 0.0f }, acceleration{ 0.0f }, turnRotation{ 0.0f },
	rotateWithVelocity{ false }, glow{ false }, mainColor{ Color::white }, subColor{ Color::white },
	nextInfo{ nullptr }, bulletEvent{ BulletEvent() }, gravity{ Vector2(0.0f) }
{}

cs::Extent::Extent() :
	width{ 0.0f }, height{ 0.0f }, offset{ Vector2(0.0f) }
{}

cs::BulletType::BulletType() :
	damage{ 0 }, texOffset{ Vector2(0.0f) }, texExtents{ Vector2(0.0f) }, collider{ nullptr }
{}

cs::BulletShaderParams::BulletShaderParams() :
	position{ Vector2(0.0f) }, rotation{ 0.0f }, mainColor{ Color::white }, subColor{ Color::white }
{}
