#include "PhysicsServer.h"

#include "PhysicsBody.h"
#include "Collider.h"
#include "StaticBody.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "SphereCollider.h"
#include "PolygonCollider.h"
#include "GameObject.h"

#include "Debug.h"

cs::RaycastHit cs::PhysicsServer::Raycast(const Vector3 origin, const Vector3 direction, const float distance, const OBB& obb, const Matrix4x4& matrix)
{
	RaycastHit _hit{};
	
	float _minDistance{ 0.0f }, _maxDistance{ distance };
	const Vector3 _delta{ Vector3(matrix[3]) - origin }, _xAxis{ matrix[0] }, _yAxis{ matrix[1] }, _zAxis{ matrix[2] };
	bool _xTest{ TestRayAgainstAxis(_xAxis, _delta, direction, _minDistance, _maxDistance, obb.minExtent.x, obb.maxExtent.x) };
	bool _yTest{ TestRayAgainstAxis(_yAxis, _delta, direction, _minDistance, _maxDistance, obb.minExtent.y, obb.maxExtent.y) };
	bool _zTest{ TestRayAgainstAxis(_zAxis, _delta, direction, _minDistance, _maxDistance, obb.minExtent.z, obb.maxExtent.z) };
	
	_hit.valid = _xTest && _yTest && _zTest;
	_hit.distance = _minDistance * _hit.valid; // if we didn't hit anything, the distance is 0

	return _hit;
}

void cs::PhysicsServer::Step()
{
	Test();
	Solve();
}

void cs::PhysicsServer::Test()
{
	for (auto* a : bodies)
	{
		for (auto* b : bodies)
		{
			if (a == b)
				break;

			CollisionInfo _info{ PhysicsBody::CompareBodies(a, b) };

			if (_info.valid)
				collisionQueue.emplace_back(Collision(a, b, _info));
		}
	}
}

void cs::PhysicsServer::Solve()
{
	for (auto& collision : collisionQueue)
	{
		// for now we will just do this... later we can change this to a double dispatch
		// this is a very crude way to implement a solver
		if (collision.bodyA->bodyType == PhysicsBodyComponent::BodyType::RIGID && collision.bodyB->bodyType == PhysicsBodyComponent::BodyType::RIGID)
		{
			RigidbodyComponent* _rbA{ reinterpret_cast<RigidbodyComponent*>(collision.bodyA) };
			RigidbodyComponent* _rbB{ reinterpret_cast<RigidbodyComponent*>(collision.bodyB) };
		}
		else if (collision.bodyA->bodyType == PhysicsBodyComponent::BodyType::RIGID && collision.bodyB->bodyType == PhysicsBodyComponent::BodyType::STATIC)
		{
			RigidbodyComponent* _rbA{ reinterpret_cast<RigidbodyComponent*>(collision.bodyA) };
			StaticBodyComponent* _sbB{ reinterpret_cast<StaticBodyComponent*>(collision.bodyB) };

			//Vector3 _bouncyForce{ glm::reflect(_rbA->velocity, collision.info.normal) * 10.0f };
			TransformComponent& _tr{ _rbA->gameObject->GetComponent<TransformComponent>() };
			_tr.position += collision.info.normal * collision.info.errorLength;
			Vector3 _proj{ Mathf::Project(collision.info.normal, _rbA->velocity) };
			_rbA->velocity = -_proj;

			//_rbA->AddForce(collision.info.normal * collision.info.errorLength * 100.0f);
		}
		else if (collision.bodyA->bodyType == PhysicsBodyComponent::BodyType::STATIC && collision.bodyB->bodyType == PhysicsBodyComponent::BodyType::RIGID)
		{
			StaticBodyComponent* _sbA{ reinterpret_cast<StaticBodyComponent*>(collision.bodyA) };
			RigidbodyComponent* _rbB{ reinterpret_cast<RigidbodyComponent*>(collision.bodyB) };

			TransformComponent& _tr{ _rbB->gameObject->GetComponent<TransformComponent>() };
			_tr.position += collision.info.normal * collision.info.errorLength;
			_rbB->velocity += 1.2f * collision.info.normal * _rbB->velocity;

			//_rbB->velocity = glm::reflect(_rbB->velocity, collision.info.normal);
			//_rbB->velocity = Vector3(0.0f, 0.0f, 0.0f);
			//_rbB->AddForce(collision.info.normal * collision.info.errorLength * 100.0f);
		}
	}

	collisionQueue.clear();
}

bool cs::PhysicsServer::TestRayAgainstAxis(const Vector3 axis, const Vector3& delta, const Vector3& direction, float& minDistance, float& maxDistance, const float& obbMin, const float& obbMax)
{
	float _e{ Mathf::DotProduct(axis, delta) }, _f{ Mathf::DotProduct(direction, axis) };

	float _t1{ 0.0f };
	float _t2{ 0.0f };

	if (_f != 0.0f)
	{
		_t1 = (_e + obbMin) / _f;
		_t2 = (_e + obbMax) / _f;
	}
	else
		return -_e + obbMin > 0.0f || -_e + obbMax < 0.0f;

	// swap if t1 is larger than t2
	if (_t1 > _t2)
	{
		float _w{ _t1 };
		_t1 = _t2;
		_t2 = _w;
	}

	minDistance = _t1 * (_t1 > minDistance) + minDistance * (_t1 <= minDistance);
	maxDistance = _t2 * (_t2 < maxDistance) + maxDistance * (_t2 >= maxDistance);

	return minDistance <= maxDistance;
}

cs::CollisionInfo cs::collision_tests::SphereSphereIntersection(const TransformComponent* t, const SphereColliderComponent* c, const TransformComponent* ot, const SphereColliderComponent* oc)
{
	CollisionInfo _info{};

	Vector3 _localSpace{ ot->position - t->position };
	_info.normal = glm::normalize(_localSpace);
	_info.a = _info.normal * c->radius;
	_info.b = -_info.normal * oc->radius;
	_info.length = glm::length(_localSpace);
	_info.errorLength = _info.length - (c->radius + oc->radius);
	_info.valid = _info.errorLength <= 0.0f;

	return _info;
}

cs::CollisionInfo cs::collision_tests::SpherePolygonIntersection(const TransformComponent* t, const SphereColliderComponent* c, const TransformComponent* ot, const PolygonColliderComponent* oc)
{
	CollisionInfo _info{};

	// currently only supports convex shapes (will hopefully support concave shapes in the future)
	for (auto& plane : oc->GetPlanes())
	{
		_info.normal = plane.normal;
		_info.a = c->radius * _info.normal + t->position;
		_info.errorLength = Mathf::Project(_info.a, plane) - c->radius;
		_info.valid = _info.valid && _info.errorLength <= 0.0f;
	}

	return _info;
}

cs::Collision::Collision(PhysicsBodyComponent* a, PhysicsBodyComponent* b, CollisionInfo newInfo) :
	bodyA{ a }, bodyB{ b }, info{ newInfo }
{}
