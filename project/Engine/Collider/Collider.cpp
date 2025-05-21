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

// ---------------------------------------------------------
// AABB同士の最小押し戻しベクトルを取得
// ---------------------------------------------------------
Float3 AABBCollider::GetPushBackVector(const AABBCollider& other) const 
{ 
    Float3 centerA = (min_ + max_) * 0.5f;
	Float3 centerB = (other.min_ + other.max_) * 0.5f;

    Float3 halfSizeA = (max_ - min_) * 0.5f;
	Float3 halfSizeB = (other.max_ - other.min_) * 0.5f;

    Float3 delta = centerA - centerB;
	Float3 overlap = {
        (halfSizeA.x + halfSizeB.x) - std::abs(delta.x), 
        (halfSizeA.y + halfSizeB.y) - std::abs(delta.y), 
        (halfSizeA.z + halfSizeB.z) - std::abs(delta.z)
    };

    if (overlap.x < overlap.y && overlap.x < overlap.z) {
		return {(delta.x > 0 ? overlap.x : -overlap.x), 0.0f, 0.0f};
	} else if (overlap.y < overlap.z) {
		return {0.0f, (delta.y > 0 ? overlap.y : -overlap.y), 0.0f};
	} else {
		return {0.0f, 0.0f, (delta.z > 0 ? overlap.z : -overlap.z)};
    }
}
