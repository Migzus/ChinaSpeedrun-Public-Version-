#pragma once

#include "Color.h"
#include "Mathf.h"

#include "VulkanEngineRenderer.h"
#include "RenderComponent.h"
#include "b2/box2d.h"

#include <queue>
#include <vector>
#include <unordered_map>

namespace cs
{
	// A helper for the borders
	struct Extent
	{
		float width, height;
		Vector2 offset;

		Extent();
	};

	// The event system in the bullet system 
	struct BulletEvent
	{
		// these are just variables for now, but will probably change to some extent
		// change as in we might structure it differently

		float timeout;
		bool hitTopBorder;
		bool hitBottomBorder;
		bool hitLeftBorder;
		bool hitRightBorder;
		bool hitPlayer;
		bool hitEnemy;
		bool hitOtherEvent; // this is when we attach event logic to other colliders in the scene
	};
	
	// Types of bullets, we might want different textures, collision types and sizes ect.
	struct BulletType
	{
		int damage;
		Vector2 texOffset, texExtents;
		class ColliderType* collider;

		BulletType();
	};

	// The information we give to each bullet instance when we ask for one to spawn
	struct BulletInfo
	{
		Vector2 position;
		Vector2 gravity;
		float rotation;
		float rotationOffset;
		float speed;
		float acceleration;
		float turnRotation;
		bool rotateWithVelocity;
		bool glow;
		Color mainColor;
		Color subColor;

		const BulletType* type;
		BulletEvent bulletEvent;

		BulletInfo* nextInfo; // once the bullet triggers an event we go to the next info, if this pointer is 0x0 then we despawn the bullet

		BulletInfo(BulletType* type);
	};

	class BulletManagerComponent;

	struct BulletShaderParams
	{
		Vector2 position;
		float rotation;
		Color mainColor, subColor;
		//Vector2 texPixelOffset, texPixelSize;

		BulletShaderParams();
	};

	// Here is where the actual bullet logic happens, by using the bullet info
	class Bullet
	{
	public:
		friend BulletManagerComponent;

		BulletManagerComponent* manager;
		float turnRate, acceleration, speed, spin, rotationOffset, rotateWithVelocity, deltaRotation;
		Vector2 gravity;

		Bullet(BulletManagerComponent* manager);

		void BorderCheck();
		void Update(const uint32_t& index);
		void SetInfo(const BulletInfo& info);

	protected:
		float currentSpin;
		Vector2 velocity;

	private:
		BulletShaderParams params;
		UniformBufferObject ubo;
	};

	class Texture;

	// The manager, does what the name says
	class BulletManagerComponent : public RenderComponent
	{
	public:
		friend Bullet;
		friend VulkanEngineRenderer;

		int bulletCapacity;
		Extent mainBorder, absoluteBorder;
		std::unordered_map<std::string, BulletType*> types;
		Texture* mainTex;
		Texture* subTex;
		UniformBufferObject* ubos;
		BulletShaderParams* bulletShaderParams;

		BulletManagerComponent();

		virtual void Init() override;
		virtual void ImGuiDrawComponent() override;
		virtual void VulkanDraw(VkCommandBuffer& commandBuffer, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer);

		void CreateSystem();

		void CreateBorders(const float& width, const float& height, const float& margin, const Vector2& offset = Vector2(0.0f));
		void UpdateUBO(class CameraBase& camera);
		void Update();
		void SpawnCircle(BulletInfo& info, const uint16_t bulletCount, const float radius = 0.0f, const float spacing = 0.0f);
		Bullet* SpawnBullet(const BulletInfo& info);
		void DespawnBullet(const uint64_t& index);
		void DespawnBullet(Bullet* bullet);

		void DestroyBuffer();
		void DestroySystem();

		~BulletManagerComponent();

	private:
		b2BodyDef definition;
		b2Body* body;
		const class Mesh* quadSprite;
		VulkanBufferInfo bufferInfo;

		std::vector<Bullet*> activeBullets;
		std::queue<Bullet*> readyBullets;

		void CreateBulletBuffer();
	};
}
