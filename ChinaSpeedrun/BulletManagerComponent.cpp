#include "BulletManagerComponent.h"

#include "imgui.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Time.h"
#include "Input.h"

#include "Debug.h"

cs::BulletManagerComponent::BulletManagerComponent() :
	body{ nullptr }, quadSprite{ Mesh::CreateDefaultPlane(Vector2(0.5f)) }
{
	definition.type = b2_staticBody;
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

		Mathf::Clamp(bulletCapacity, 0, 10000);

		ImGui::TreePop();
	}
}

void cs::BulletManagerComponent::VulkanDraw(VkCommandBuffer& commandBuffer, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer)
{
	VkDeviceSize _size{ (VkDeviceSize)sizeof(UniformBufferSpriteObject) };

	/*
	void* _data;
	vkMapMemory(device, uniformBuffersMemory[index], 0, (VkDeviceSize)sizeof(UniformBufferSpriteObject), 0, &_data);
	memcpy(_data, ubos, (size_t)UniformBufferObject::GetByteSize());
	vkUnmapMemory(device, uniformBuffersMemory[index]);
	*/

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline);

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &quadSprite->vertexBufferOffset);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, quadSprite->indexBufferOffset, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->shader->layout, 0, 1, &descriptorSets[index], 0, nullptr);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(quadSprite->GetIndices().size()), activeBullets.size(), 0, 0, 0);
}

void cs::BulletManagerComponent::CreateSystem()
{
	for (size_t i{ 0 }; i < bulletCapacity; i++)
		readyBullets.push(new Bullet);
}

void cs::BulletManagerComponent::Update()
{
	if (Input::GetActionPressed("accept"))
	{
		BulletInfo _info{ BulletInfo(types["circle"]) };

		Debug::LogSuccess("Spawned BULLETS!!");

		_info.mainColor = Color::lime;
		_info.subColor = Color::red;
		_info.speed = 2.0f;

		SpawnCircle(_info, 2);
	}

	for (auto* bullet : activeBullets)
		bullet->Update();
}

void cs::BulletManagerComponent::SpawnCircle(BulletInfo& info, const uint16_t bulletCount, const float radius, const float overrideSpacing)
{
	const Vector2 _origin{ info.position };
	const float _rotationOffset{ Mathf::TAU / float(bulletCount) };
	const float _spacing{ overrideSpacing < 0.0f ? 1.0f : overrideSpacing };

	for (uint16_t i{ 0 }; i < bulletCount; i++)
	{
		const float _currentRotation{ _rotationOffset * i * _spacing };

		info.position = _origin + Vector2(cos(_currentRotation), sin(_currentRotation)) * radius;
		info.rotation = _currentRotation;

		SpawnBullet(info);
	}
}

cs::Bullet* cs::BulletManagerComponent::SpawnBullet(const BulletInfo& info)
{
	if (readyBullets.empty())
		return nullptr;

	Bullet* _newBullet{ readyBullets.front() };

	readyBullets.pop();
	_newBullet->SetInfo(info);
	activeBullets.push_back(_newBullet);

	return _newBullet;
}

void cs::BulletManagerComponent::DespawnBullet(const uint64_t index)
{
	Bullet* _deactivatedBullet{ activeBullets[index] };
	activeBullets.erase(activeBullets.begin() + index);

	_deactivatedBullet->SetActive(false);

	readyBullets.push(_deactivatedBullet);
}

void cs::BulletManagerComponent::DespawnBullet(Bullet* bullet)
{
	std::vector<Bullet*>::iterator _it{ std::find(activeBullets.begin(), activeBullets.end(), bullet) };

	if (_it != activeBullets.end())
		DespawnBullet(std::distance(activeBullets.begin(), _it));
}

void cs::BulletManagerComponent::DestroySystem()
{
	for (auto bullet : activeBullets)
		delete bullet;

	activeBullets.clear();

	for (size_t i{ 0 }; i < readyBullets.size(); i++)
	{
		delete readyBullets.front();
		readyBullets.pop();
	}
}

cs::BulletManagerComponent::~BulletManagerComponent()
{
	DestroySystem();
}

cs::Bullet::Bullet() :
	rotation{ 0.0f }, turnRate{ 0.0f }, acceleration{ 0.0f }, speed{ 0.0f },
	spin{ 0.0f }, gravity{ Vector2(0.0f) }, currentSpin{ 0.0f }, velocity{ Vector2(0.0f) },
	ubso{ UniformBufferSpriteObject() }, position{ Vector3(0.0f) }
{}

void cs::Bullet::Update()
{
	velocity += gravity * Time::deltaTime;
	speed += acceleration * Time::deltaTime;
	currentSpin += spin * Time::deltaTime;
	rotation += turnRate * Time::deltaTime;

	Vector2 _finalVelocity{ velocity + Vector2(cos(rotation), sin(rotation)) };
	
	position += _finalVelocity * speed * Time::deltaTime;
	
	Matrix4x4 _rotation{ Matrix4x4(1.0f) };

	_rotation[0][0] = cos(rotation);
	_rotation[0][1] = -sin(rotation);
	_rotation[1][0] = sin(rotation);
	_rotation[1][1] = cos(rotation);

	ubso.model = glm::translate(_rotation, Vector3(position, 0.0f));
}

void cs::Bullet::SetActive(const bool status)
{

}

void cs::Bullet::SetInfo(const BulletInfo& info)
{
	rotation = info.rotation;
	position = info.position;
}

cs::UniformBufferSpriteObject::UniformBufferSpriteObject() :
	model{ Matrix4x4(1.0f) }, view{ Matrix4x4(1.0f) }, proj{ Matrix4x4(1.0f) }, spriteScale{ Vector2(0.0f) }
{}

cs::BulletInfo::BulletInfo(BulletType* type) :
	type{ type }, rotation{ 0.0f }, speed{ 0.0f }, position{ Vector2(0.0f) },
	rotationOffset{ 0.0f }, acceleration{ 0.0f }, turnRotation{ 0.0f },
	rotateWithVelocity{ false }, glow{ false }, mainColor{ Color() }, subColor{ Color() },
	nextInfo{ nullptr }, bulletEvent{ BulletEvent() }
{}

cs::Extent::Extent() :
	width{ 0.0f }, height{ 0.0f }, offset{ Vector2(0.0f) }
{}

cs::BulletType::BulletType() :
	damage{ 0 }, texOffset{ Vector2(0.0f) }, texExtents{ Vector2(0.0f) }, collider{ nullptr }
{}
