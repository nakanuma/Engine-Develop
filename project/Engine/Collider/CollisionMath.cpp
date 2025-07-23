#include "CollisionMath.h"

// C++
#include <algorithm>

// Engine
#include <MyMath.h>
#include <Collider/Collider.h>

// ---------------------------------------------------------
// Sphere vs Sphere
// ---------------------------------------------------------
bool CollisionMath::CheckSphereToSphere(const SphereCollider* a, const SphereCollider* b)
{
    // 2つの球の中心点間の距離を求める
    Float3 diff = a->center_ - b->center_;
    float distSq = Float3::Dot(diff, diff);
    float radiusSum = a->radius_ + b->radius_;

    // 半径の合計よりも短ければ衝突
    return distSq <= (radiusSum * radiusSum);
}

// ---------------------------------------------------------
// Sphere vs AABB
// ---------------------------------------------------------
bool CollisionMath::CheckSphereToAABB(const SphereCollider* sphere, const AABBCollider* aabb)
{
    // 最近接点を求める
    Float3 closestPoint{
        std::clamp(sphere->center_.x, aabb->min_.x, aabb->max_.x),
        std::clamp(sphere->center_.y, aabb->min_.y, aabb->max_.y),
        std::clamp(sphere->center_.z, aabb->min_.z, aabb->max_.z)
    };
    // 最近接点と球の中心との距離を求める
    Float3 diff = sphere->center_ - closestPoint;
    float distSq = Float3::Dot(diff, diff);

    // 距離が半径より小さければ衝突
    return distSq <= (sphere->radius_ * sphere->radius_);
}

// ---------------------------------------------------------
// AABB vs AABB
// ---------------------------------------------------------
bool CollisionMath::CheckAABBToAABB(const AABBCollider* a, const AABBCollider* b)
{
    return
        (a->min_.x <= b->max_.x && a->max_.x >= b->min_.x) &&
        (a->min_.y <= b->max_.y && a->max_.y >= b->min_.y) &&
        (a->min_.z <= b->max_.z && a->max_.z >= b->min_.z);
}

// ---------------------------------------------------------
// AABB vs OBB
// ---------------------------------------------------------
bool CollisionMath::CheckAABBToOBB(const AABBCollider* aabb, const OBBCollider* obb)
{
    return false;
}

// ---------------------------------------------------------
// OBB vs OBB
// ---------------------------------------------------------
bool CollisionMath::CheckOBBToOBB(const OBBCollider* a, const OBBCollider* b)
{
    return false;
}

// ---------------------------------------------------------
// OBB vs Sphere
// ---------------------------------------------------------
bool CollisionMath::CheckOBBToSphere(const OBBCollider* obb, const SphereCollider* sphere)
{ 
    // 球の中心をOBB空間に射影
    Float3 d = sphere->center_ - obb->center_; 

    // 各軸への投影距離を箱の半サイズ以内で計算
	Float3 closestPoint = obb->center_;

    closestPoint += obb->xAxis_ * std::clamp(Float3::Dot(d, obb->xAxis_), -obb->size_.x, obb->size_.x);
	closestPoint += obb->yAxis_ * std::clamp(Float3::Dot(d, obb->yAxis_), -obb->size_.y, obb->size_.y);
	closestPoint += obb->zAxis_ * std::clamp(Float3::Dot(d, obb->zAxis_), -obb->size_.z, obb->size_.z);

    Float3 vecToSphere = sphere->center_ - closestPoint;
	float distSq = Float3::LengthSq(vecToSphere);

    return distSq <= sphere->radius_ * sphere->radius_;
}
