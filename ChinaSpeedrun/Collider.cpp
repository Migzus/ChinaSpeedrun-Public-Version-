#include "Collider.h"

#include "PhysicsServer.h"
#include "Transform.h"
#include "SphereCollider.h"
#include "PolygonCollider.h"

void cs::ColliderComponent::Init()
{

}

cs::CollisionInfo cs::ColliderComponent::Intersect(const TransformComponent* transform, const ColliderComponent* otherCollider, const TransformComponent* otherTransform) const
{
    return {};
}

cs::CollisionInfo cs::ColliderComponent::Intersect(const TransformComponent* transform, const SphereColliderComponent* otherCollider, const TransformComponent* otherTransform) const
{
    return {};
}

cs::CollisionInfo cs::ColliderComponent::Intersect(const TransformComponent* transform, const PolygonColliderComponent* otherCollider, const TransformComponent* otherTransform) const
{
    return {};
}

void cs::ColliderComponent::ImGuiDrawComponent()
{}
