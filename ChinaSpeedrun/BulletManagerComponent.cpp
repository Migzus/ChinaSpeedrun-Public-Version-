#include "BulletManagerComponent.h"

void cs::BulletManagerComponent::Init()
{

}

void cs::BulletManagerComponent::CreateSystem()
{
	for (size_t i{ 0 }; i < bulletCapacity; i++)
		readyBullets.push_back(new Bullet);
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

	Bullet* _newBullet{ readyBullets.back() };

	readyBullets.pop_back();

	// Prepare the new bullet here

	activeBullets.push_back(_newBullet);

	return _newBullet;
}

void cs::BulletManagerComponent::DespawnBullet(const uint64_t index)
{
	Bullet* _deactivatedBullet{ activeBullets[index] };
	activeBullets.erase(activeBullets.begin() + index);

	_deactivatedBullet->SetActive(false);

	readyBullets.push_back(_deactivatedBullet);
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

	for (auto bullet : readyBullets)
		delete bullet;

	readyBullets.clear();
}

cs::BulletManagerComponent::~BulletManagerComponent()
{
	DestroySystem();
}

void cs::Bullet::SetActive(const bool status)
{

}

void cs::Bullet::SetInfo(BulletInfo* info)
{
	this->info = *info;

	// set the new position
	// rotation etc.
}
