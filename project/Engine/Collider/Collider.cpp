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
    // vs OBB
	else if (other->GetType() == "OBB") {
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
    else if (other->GetType() == "AABB") {
        auto* aabb = dynamic_cast<AABBCollider*>(other);
        return CollisionMath::CheckAABBToAABB(this, aabb);
    }
    // vs OBB
    else if (other->GetType() == "OBB") {
        auto* obb = dynamic_cast<OBBCollider*>(other);
        return CollisionMath::CheckAABBToOBB(this, obb);
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

// ---------------------------------------------------------
// SphereとAABBの接触面の法線を取得
// ---------------------------------------------------------
Float3 AABBCollider::GetContactNormalFromSphere(const Float3& sphereCenter) const
{
    Float3 clamped;
    clamped.x = std::max(min_.x, std::min(sphereCenter.x, max_.x));
    clamped.y = std::max(min_.y, std::min(sphereCenter.y, max_.y));
    clamped.z = std::max(min_.z, std::min(sphereCenter.z, max_.z));

    Float3 delta = sphereCenter - clamped;

    if (std::abs(delta.x) > std::abs(delta.y) && std::abs(delta.x) > std::abs(delta.z)) {
        return { delta.x > 0 ? 1.0f : -1.0f, 0.0f, 0.0f };
    } else if (std::abs(delta.y) > std::abs(delta.z)) {
        return { 0.0f, delta.y > 0 ? 1.0f : -1.0f, 0.0f };
    } else {
        return { 0.0f, 0.0f, delta.z > 0 ? 1.0f : -1.0f };
    }
}

// ---------------------------------------------------------
// OBBコライダーの衝突判定
// ---------------------------------------------------------
bool OBBCollider::CheckCollision(Collider* other) {
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
