#pragma once

#include "Color.h"
#include "Mathf.h"

#include "RenderComponent.h"

#include <vector>
#include <unordered_map>

namespace cs
{
	// A helper for the borders
	struct Extent
	{
		float width, height;
		Vector2 offset;
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
		class Texture* main;
		class Texture* sub;
		class ColliderType* collider;
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
		Color main;
		Color sub;

		BulletType* type;
		BulletEvent event;

		BulletInfo* nextInfo; // once the bullet triggers an event we go to the next info, if this pointer is 0x0 then we despawn the bullet
	};

	// Here is where the actual bullet logic happens, by using the bullet info
	class Bullet
	{
	public:
		void SetActive(const bool status);
		void SetInfo(BulletInfo* info);

	protected:
		Vector2 velocity;

	private:
		BulletInfo info;
	};

	// The manager, does what the name says
	class BulletManagerComponent : public RenderComponent
	{
	public:
		uint64_t bulletCapacity;
		Extent mainBorder, absoluteBorder;

		virtual void Init() override;

		void CreateSystem();

		void SpawnCircle(BulletInfo& info, const uint16_t bulletCount, const float radius = 0.0f, const float overrideSpacing = -1.0f);
		Bullet* SpawnBullet(const BulletInfo& info);
		void DespawnBullet(const uint64_t index);
		void DespawnBullet(Bullet* bullet);

		void DestroySystem();

		~BulletManagerComponent();

	private:
		std::vector<Bullet*> readyBullets, activeBullets;
	};
}
