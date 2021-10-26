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
		}
		else if (collision.bodyA->bodyType == PhysicsBodyComponent::BodyType::STATIC && collision.bodyB->bodyType == PhysicsBodyComponent::BodyType::RIGID)
		{
			StaticBodyComponent* _sbA{ reinterpret_cast<StaticBodyComponent*>(collision.bodyA) };
			RigidbodyComponent* _rbB{ reinterpret_cast<RigidbodyComponent*>(collision.bodyB) };


			TransformComponent& _tr{ _rbB->gameObject->GetComponent<TransformComponent>() };
			_tr.position += collision.info.normal * collision.info.errorLength;

			//_rbB->velocity = glm::reflect(_rbB->velocity, collision.info.normal);
			//_rbB->velocity = Vector3(0.0f, 0.0f, 0.0f);
			_rbB->AddForce(collision.info.normal * collision.info.errorLength * 100.0f);
		}
	}

	collisionQueue.clear();
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
	return CollisionInfo();
}

cs::Collision::Collision(PhysicsBodyComponent* a, PhysicsBodyComponent* b, CollisionInfo newInfo) :
	bodyA{ a }, bodyB{ b }, info{ newInfo }
{}
