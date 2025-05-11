#include "Collider.h"

// Engine
#include <Collider/CollisionMath.h>

// ---------------------------------------------------------
// Sphereコライダーの衝突判定
// ---------------------------------------------------------
bool SphereCollider::CheckCollision(Collider* other)
{
    // vs Sphere
    if (other->GetType() == "Sphere") {
        auto* sphere = dynamic_cast<SphereCollider*>(other);
        return CollisionMath::CheckSphereToSphere(this, sphere);
    }
    // vs AABB
    else if (other->GetType() == "AABB") {
        auto* aabb = dynamic_cast<AABBCollider*>(other);
        return CollisionMath::CheckSphereToAABB(this, aabb);
    }

    return false;
}

// ---------------------------------------------------------
// AABBコライダーの衝突判定
// ---------------------------------------------------------
bool AABBCollider::CheckCollision(Collider* other)
{
    // vs Sphere
    if (other->GetType() == "Sphere") {
        auto* sphere = dynamic_cast<SphereCollider*>(other);
        return CollisionMath::CheckSphereToAABB(sphere, this);
    }
    // vs AABB
    else if (other->GetType() == "AABB") {
        auto* aabb = dynamic_cast<AABBCollider*>(other);
        return CollisionMath::CheckAABBToAABB(this, aabb);
    }

    return false;
}
