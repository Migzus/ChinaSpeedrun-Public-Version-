#include "PhysicsServer.h"

#include "PhysicsBody.h"
#include "Collider.h"
#include "StaticBody.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "SphereCollider.h"
#include "PolygonCollider.h"
#include "PlaneCollider.h"
#include "Vertex.h"
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

void cs::PhysicsServer::Reset()
{
	for (auto* body : bodies)
	{
		switch (body->bodyType)
		{
		case PhysicsBodyComponent::BodyType::RIGID:
		{
			RigidbodyComponent* _rb{ static_cast<RigidbodyComponent*>(body) };
			_rb->velocity = Vector3(0.0f);
			break;
		}
		case PhysicsBodyComponent::BodyType::STATIC:
		{
			StaticBodyComponent* _sb{ static_cast<StaticBodyComponent*>(body) };
			break;
		}
		case PhysicsBodyComponent::BodyType::AREA:
			break;
		case PhysicsBodyComponent::BodyType::KINEMATIC:
			break;
		case PhysicsBodyComponent::BodyType::UNDEFINED:
			break;
		}
	}
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
			if (a == b || (a->bodyType == PhysicsBodyComponent::BodyType::STATIC && b->bodyType == PhysicsBodyComponent::BodyType::STATIC))
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
			TransformComponent& _trA{ _rbA->gameObject->GetComponent<TransformComponent>() };
			TransformComponent& _trB{ _rbB->gameObject->GetComponent<TransformComponent>() };
			const Vector3 _projA{ Mathf::Project(collision.info.normal, _rbA->velocity) };
			const Vector3 _projB{ Mathf::Project(collision.info.normal, _rbB->velocity) };
			const float _pushBackDistance{ collision.info.errorLength * 0.5f };

			_trA.position -= collision.info.normal * _pushBackDistance;
			_trB.position += collision.info.normal * _pushBackDistance;
			_rbA->velocity -= _projA / _rbA->mass;
			_rbB->velocity -= _projB / _rbB->mass;
		}
		else if (collision.bodyA->bodyType == PhysicsBodyComponent::BodyType::RIGID && collision.bodyB->bodyType == PhysicsBodyComponent::BodyType::STATIC)
		{
			RigidbodyComponent* _rbA{ reinterpret_cast<RigidbodyComponent*>(collision.bodyA) };
			StaticBodyComponent* _sbB{ reinterpret_cast<StaticBodyComponent*>(collision.bodyB) };
			TransformComponent& _tr{ _rbA->gameObject->GetComponent<TransformComponent>() };
			const Vector3 _proj{ Mathf::Project(collision.info.normal, _rbA->velocity) };

			_tr.position -= collision.info.normal * collision.info.errorLength;
			_rbA->velocity -= _proj / _rbA->mass;
		}
		else if (collision.bodyA->bodyType == PhysicsBodyComponent::BodyType::STATIC && collision.bodyB->bodyType == PhysicsBodyComponent::BodyType::RIGID)
		{
			StaticBodyComponent* _sbA{ reinterpret_cast<StaticBodyComponent*>(collision.bodyA) };
			RigidbodyComponent* _rbB{ reinterpret_cast<RigidbodyComponent*>(collision.bodyB) };
			TransformComponent& _tr{ _rbB->gameObject->GetComponent<TransformComponent>() };
			const Vector3 _proj{ Mathf::Project(collision.info.normal, _rbB->velocity) };

			_tr.position -= collision.info.normal * collision.info.errorLength;
			_rbB->velocity -= _proj / _rbB->mass;
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
	const auto& _vertices{ oc->GetMeshCollider().vertices };
	const auto& _indices{ oc->GetMeshCollider().indices };
	uint32_t _collisionCount{ 0 };

	for (size_t i{ 0 }; i < _indices.size(); i += 3)
	{
		const Vector3 _pointA{ _vertices[_indices[i]].position + ot->position }; // this becomes the origin
		const Vector3 _pointB{ _vertices[_indices[i + 1]].position - _pointA };
		const Vector3 _pointC{ _vertices[_indices[i + 2]].position - _pointA };
		const Vector3 _pointP{ t->position - _pointA };

		const float _w1{ (_pointP.z * _pointC.x - _pointP.x * _pointC.z) / (_pointB.z * _pointC.x - _pointB.x * _pointC.z) };
		const float _w2{ (_pointP.z - _w1 * _pointB.z) / _pointC.z };

		if (_w1 >= 0.0f && _w2 >= 0.0f && _w1 + _w2 <= 1.0f)
		{
			const Vector3 _offsetB{ _pointB - _pointA };
			const Vector3 _offsetC{ _pointC - _pointA };

			_info.normal += glm::normalize(Mathf::CrossProduct(_offsetB, _offsetC));
			_info.a = -_info.normal * c->radius + _pointP;
			_info.b = _offsetB * _w1 + _offsetC * _w2;
			_info.length = glm::length(_pointP - _info.b);
			_info.errorLength = _info.length - glm::length(_info.a - _info.b);
			_info.valid |= _info.errorLength <= 0.0f;
			_collisionCount++;
		}
	}

	_info.normal /= _collisionCount == 0 ? 1.0f : (float)_collisionCount; // safety net, so we don't divide by zero... ever

	return _info;
}

cs::CollisionInfo cs::collision_tests::SpherePlaneIntersection(const TransformComponent* t, const SphereColliderComponent* c, const TransformComponent* ot, const PlaneColliderComponent* oc)
{
	CollisionInfo _info{};

	_info.normal = -oc->plane.normal;
	_info.a = c->radius * _info.normal + t->position;
	_info.errorLength = Mathf::Project(_info.a, oc->plane) - c->radius;
	_info.valid = _info.valid && _info.errorLength <= 0.0f;

	return _info;
}

cs::Collision::Collision(PhysicsBodyComponent* a, PhysicsBodyComponent* b, CollisionInfo newInfo) :
	bodyA{ a }, bodyB{ b }, info{ newInfo }
{}
