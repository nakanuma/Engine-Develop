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
    if (other->GetType() == "AABB") {
        auto* aabb = dynamic_cast<AABBCollider*>(other);
        return CollisionMath::CheckSphereToAABB(this, aabb);
    }

    // vs OBB
    if (other->GetType() == "OBB") {
        auto* obb = dynamic_cast<OBBCollider*>(other);
        return CollisionMath::CheckOBBToSphere(obb, this);
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
    if (other->GetType() == "AABB") {
        auto* aabb = dynamic_cast<AABBCollider*>(other);
        return CollisionMath::CheckAABBToAABB(this, aabb);
    }

    // bs OBB
    if (other->GetType() == "OBB") {
        auto* obb = dynamic_cast<OBBCollider*>(other);
        return CollisionMath::CheckAABBToOBB(this, obb);
    }

    return false;
}

// ---------------------------------------------------------
// OBBコライダーの衝突判定
// ---------------------------------------------------------
bool OBBCollider::CheckCollision(Collider* other)
{
    // vs Sphere
    if (other->GetType() == "Sphere") {
        auto* sphere = dynamic_cast<SphereCollider*>(other);
        return CollisionMath::CheckOBBToSphere(this, sphere);
    }

    // vs AABB
    if (other->GetType() == "AABB") {
        auto* aabb = dynamic_cast<AABBCollider*>(other);
        return CollisionMath::CheckAABBToOBB(aabb, this);
    }

    // vs OBB
    if (other->GetType() == "OBB") {
        auto* obb = dynamic_cast<OBBCollider*>(other);
        return CollisionMath::CheckOBBToOBB(this, obb);
    }

    return false;
}
