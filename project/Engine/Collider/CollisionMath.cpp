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
    // AABB を中心サイズ・半サイズ・単位軸付きのOBBとして扱う
    OBBCollider tempOBB;
    tempOBB.center_ = (aabb->min_ + aabb->max_) * 0.5f;
    tempOBB.halfSize_ = (aabb->max_ - aabb->min_) * 0.5f;
    tempOBB.axes_[0] = { 1.0f, 0.0f, 0.0f };
    tempOBB.axes_[1] = { 0.0f, 1.0f, 0.0f };
    tempOBB.axes_[2] = { 0.0f, 0.0f, 1.0f };

    return CheckOBBToOBB(&tempOBB, obb);
}

// ---------------------------------------------------------
// OBB vs OBB
// ---------------------------------------------------------
bool CollisionMath::CheckOBBToOBB(const OBBCollider* a, const OBBCollider* b)
{
    const float EPSILON = 1e-6f;

    // 各OBBの軸
    const Float3* A = a->axes_;
    const Float3* B = b->axes_;

    // 各半サイズ
    const Float3& aSize = a->halfSize_;
    const Float3& bSize = b->halfSize_;

    // 中心差ベクトル
    Float3 D = b->center_ - a->center_;

    // RとAbsRの計算
    float R[3][3], AbsR[3][3];

    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            R[i][j] = Float3::Dot(A[i], B[j]);
            AbsR[i][j] = std::abs(R[i][j]) + EPSILON;
        }
    }

    // 中心差ベクトルを A の軸に射影
    float t[3] = {
        Float3::Dot(D, A[0]),
        Float3::Dot(D, A[1]),
        Float3::Dot(D, A[2])
    };

    // SATテスト : 15軸

    // 最初の9軸テスト（A軸とB軸の組み合わせ）
    for (size_t i = 0; i < 3; ++i) {
        float ra = (i == 0 ? aSize.x : (i == 1 ? aSize.y : aSize.z));
        float rb = bSize.x * AbsR[i][0] + bSize.y * AbsR[i][1] + bSize.z * AbsR[i][2];
        if (std::abs(t[i]) > ra + rb) return false;
    }

    // 次の6軸テスト（B軸とA軸の組み合わせ）
    for (size_t i = 0; i < 3; ++i) {
        float ra = aSize.x * AbsR[0][i] + aSize.y * AbsR[1][i] + aSize.z * AbsR[2][i];
        float rb = (i == 0 ? bSize.x : (i == 1 ? bSize.y : bSize.z));
        float val = std::abs(
            t[0] * R[0][i] +
            t[1] * R[1][i] +
            t[2] * R[2][i]
        );
        if (val > ra + rb) return false;
    }

    // 衝突している
    return true;
}

// ---------------------------------------------------------
// OBB vs Sphere
// ---------------------------------------------------------
bool CollisionMath::CheckOBBToSphere(const OBBCollider* obb, const SphereCollider* sphere)
{
    // 球の中心からOBBの中心へのベクトル
    Float3 d = sphere->center_ - obb->center_;
    Float3 closest = obb->center_;

    // 各軸に対して最近接点を求める
    for (size_t i = 0; i < 3; ++i) {
        float dist = Float3::Dot(d, obb->axes_[i]);
        if (i == 0) dist = std::clamp(dist, -obb->halfSize_.x, obb->halfSize_.x);
        if (i == 1) dist = std::clamp(dist, -obb->halfSize_.y, obb->halfSize_.y);
        if (i == 2) dist = std::clamp(dist, -obb->halfSize_.z, obb->halfSize_.z);
        closest += dist * obb->axes_[i];
    }

    // 最近接点と球の中心との距離の2乗
    Float3 diff = closest - sphere->center_;
    float distSq = Float3::Dot(diff, diff);

    return distSq <= sphere->radius_ * sphere->radius_;
}
