#pragma once

#include "Color.h"
#include "Mathf.h"

#include "RenderComponent.h"
#include "b2/box2d.h"

#include <queue>
#include <vector>
#include <unordered_map>

namespace cs
{
	struct UniformBufferSpriteObject
	{
		Matrix4x4 model, view, proj;
		Vector2 spriteScale;

		UniformBufferSpriteObject();
	};

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
		// these will be preloaded when the first bullet manager spawns
		static std::unordered_map<std::string, BulletType*> bulletTypes;

		int damage;
		Vector2 texOffset, texExtents;
		class ColliderType* collider;

		BulletType();
	};

	// The information we give to each bullet instance when we ask for one to spawn
	struct BulletInfo
	{
		Vector2 position;
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

	// Here is where the actual bullet logic happens, by using the bullet info
	class Bullet
	{
	public:
		float rotation, turnRate, acceleration, speed, spin;
		Vector2 gravity, position;
		
		Bullet();

		void Update();
		void SetActive(const bool status);
		void SetInfo(const BulletInfo& info);

	protected:
		float currentSpin;
		Vector2 velocity;

	private:
		UniformBufferSpriteObject ubso;
	};

	// The manager, does what the name says
	class BulletManagerComponent : public RenderComponent
	{
	public:
		int bulletCapacity;
		Extent mainBorder, absoluteBorder;
		std::unordered_map<std::string, BulletType*> types;
		class Texture* main;
		Texture* sub;

		BulletManagerComponent();

		virtual void Init() override;
		virtual void ImGuiDrawComponent() override;
		virtual void VulkanDraw(VkCommandBuffer& commandBuffer, const size_t& index, VkBuffer& vertexBuffer, VkBuffer& indexBuffer);

		void CreateSystem();

		void Update();
		void SpawnCircle(BulletInfo& info, const uint16_t bulletCount, const float radius = 0.0f, const float overrideSpacing = -1.0f);
		Bullet* SpawnBullet(const BulletInfo& info);
		void DespawnBullet(const uint64_t index);
		void DespawnBullet(Bullet* bullet);

		void DestroySystem();

		~BulletManagerComponent();

	private:
		b2BodyDef definition;
		b2Body* body;
		const class Mesh* quadSprite;

		std::vector<Bullet*> activeBullets;
		std::queue<Bullet*> readyBullets;
	};
}
