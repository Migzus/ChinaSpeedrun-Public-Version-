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
	/**
	* An extents struct mostly used related to the bullet border of bullet systems
	*/
	struct Extent
	{
		float width, height;
		Vector2 offset;

		Extent();
	};

	/**
	* Types of bullets, we might want different textures, collision types and sizes ect.
	*/
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
	
	/**
	* Describes the information about what a bullet looks like
	*/
	struct BulletType
	{
		int damage;
		Vector2 texOffset, texExtents;
		class ColliderType* collider;

		BulletType();
	};

	/**
	* The information we give to each bullet instance when we ask for one to spawn
	*/
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

	/**
	* The shader informatin per bullet instance we give to the shader
	*/
	struct BulletShaderParams
	{
		Vector2 position;
		float rotation;
		Color mainColor, subColor;
		//Vector2 texPixelOffset, texPixelSize;

		BulletShaderParams();
	};

	/**
	* The bullet itself. This is where bullets' information simulation is stored
	*/
	class Bullet
	{
	public:
		friend BulletManagerComponent;

		BulletManagerComponent* manager;
		float turnRate, acceleration, speed, spin, rotationOffset, rotateWithVelocity, deltaRotation;
		Vector2 gravity;

		Bullet(BulletManagerComponent* manager);

		/*!
		* Test this bullet against the border
		*/
		void BorderCheck();
		/*!
		* Simulate the next step of the bullet
		*/
		void Update(const uint32_t& index);
		/*!
		* Set the spawn info (only called once the bullet has spawned)
		*/
		void SetInfo(const BulletInfo& info);

	protected:
		float currentSpin;
		Vector2 velocity;

	private:
		BulletShaderParams params;
		UniformBufferObject ubo;
	};

	class Texture;

	/*!
	* The manager manages all of the bullets. You can ask it to spawn bullets, remove bullets and so on.
	*/
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

		/*!
		* Create the bullet system
		* (Create all the bullets)
		*/
		void CreateSystem();

		/*!
		* Creates the bullet borders
		* We have a normal border and an absolute border.
		* The margin parameter describes the margin between the two.
		*/
		void CreateBorders(const float& width, const float& height, const float& margin, const Vector2& offset = Vector2(0.0f));
		void UpdateUBO(class CameraBase& camera);
		/*!
		* Update the entire system and all of its bullets
		*/
		void Update();
		/*!
		* Spawn a circle of bullets
		*/
		void SpawnCircle(BulletInfo& info, const uint16_t bulletCount, const float radius = 0.0f, const float spacing = 0.0f);
		/*!
		* Spawns a bullet
		*/
		Bullet* SpawnBullet(const BulletInfo& info);
		/*!
		* [Fastest Method]
		* Despawns a bullet from its index in the active bullets vector
		*/
		void DespawnBullet(const uint64_t& index);
		/*
		* [Slower Method]
		* Finds the bullet in the vector and despawns it
		*/
		void DespawnBullet(Bullet* bullet);

		/*!
		* Destroy the vulkan buffers for this system
		*/
		void DestroyBuffer();
		/*!
		* Destroys the entire bullet system
		*/
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
