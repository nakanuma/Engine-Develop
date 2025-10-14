#include "CollisionMath.h"

// C++
#include <algorithm>

// Engine
#include <Collider/Collider.h>

bool CollisionMath::CheckSphereToSphere(const SphereCollider* a, const SphereCollider* b) {
	// 2つの球の中心点間の距離を求める
	Float3 diff = a->center_ - b->center_;
	float distSq = Float3::Dot(diff, diff);
	float radiusSum = a->radius_ + b->radius_;

	// 半径の合計よりも短ければ衝突
	return distSq <= (radiusSum * radiusSum);
}

bool CollisionMath::CheckSphereToAABB(const SphereCollider* sphere, const AABBCollider* aabb) {
	// 最近接点を求める
	Float3 closestPoint{
	    std::clamp(sphere->center_.x, aabb->min_.x, aabb->max_.x), std::clamp(sphere->center_.y, aabb->min_.y, aabb->max_.y), std::clamp(sphere->center_.z, aabb->min_.z, aabb->max_.z)};
	// 最近接点と球の中心との距離を求める
	Float3 diff = sphere->center_ - closestPoint;
	float distSq = Float3::Dot(diff, diff);

	// 距離が半径より小さければ衝突
	return distSq <= (sphere->radius_ * sphere->radius_);
}

bool CollisionMath::CheckAABBToAABB(const AABBCollider* a, const AABBCollider* b) {
	return (a->min_.x <= b->max_.x && a->max_.x >= b->min_.x) && (a->min_.y <= b->max_.y && a->max_.y >= b->min_.y) && (a->min_.z <= b->max_.z && a->max_.z >= b->min_.z);
}

bool CollisionMath::CheckAABBToOBB(const AABBCollider* aabb, const OBBCollider* obb) {
	// AABBの中心と半サイズ
	Float3 aabbCenter = (aabb->min_ + aabb->max_) * 0.5f;
	Float3 aabbHalfSize = (aabb->max_ - aabb->min_) * 0.5f;

	// AABBをOBBの形に変換
	OBBCollider aabbAsOBB;
	aabbAsOBB.center_ = aabbCenter;
	aabbAsOBB.size_ = aabbHalfSize;

	// ワールド軸に平行な軸をセット
	aabbAsOBB.xAxis_ = {1.0f, 0.0f, 0.0f};
	aabbAsOBB.yAxis_ = {0.0f, 1.0f, 0.0f};
	aabbAsOBB.zAxis_ = {0.0f, 0.0f, 1.0f};

	// OBBvsOBBを行う
	return CheckOBBToOBB(&aabbAsOBB, obb);
}

bool CollisionMath::CheckOBBToOBB(const OBBCollider* a, const OBBCollider* b) {
	// 軸候補（分離軸）15本
	Float3 axis[15] = {
	    // Aのローカル軸
	    a->xAxis_,
	    a->yAxis_,
	    a->zAxis_,
	    // Bのローカル軸
	    b->xAxis_,
	    b->yAxis_,
	    b->zAxis_,
	    // A x B（外積軸）
	    Float3::Cross(a->xAxis_, b->xAxis_),
	    Float3::Cross(a->xAxis_, b->yAxis_),
	    Float3::Cross(a->xAxis_, b->zAxis_),

	    Float3::Cross(a->yAxis_, b->xAxis_),
	    Float3::Cross(a->yAxis_, b->yAxis_),
	    Float3::Cross(a->yAxis_, b->zAxis_),

	    Float3::Cross(a->zAxis_, b->xAxis_),
	    Float3::Cross(a->zAxis_, b->yAxis_),
	    Float3::Cross(a->zAxis_, b->zAxis_),
	};

	for (size_t i = 0; i < 15; ++i) {
		if (IsSeparatedByAxis(axis[i], a, b)) {
			return false;
		}
	}

	// すべての軸で分離していなければ衝突している
	return true;
}

bool CollisionMath::CheckOBBToSphere(const OBBCollider* obb, const SphereCollider* sphere) {
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

bool CollisionMath::IsSeparatedByAxis(const Float3& axis, const OBBCollider* obbA, const OBBCollider* obbB) {
	// 軸がゼロベクトルでないことを確認
	if (Float3::LengthSq(axis) < 1e-6f) {
		return false; // 分離軸ではない
	}

	Float3 normAxis = Float3::Normalize(axis);

	// 投影中心間距離
	float centerDist = fabsf(Float3::Dot(normAxis, obbB->center_ - obbA->center_));

	// Aの半径投影
	float rA = fabsf(Float3::Dot(normAxis, obbA->xAxis_) * obbA->size_.x) + fabsf(Float3::Dot(normAxis, obbA->yAxis_) * obbA->size_.y) + fabsf(Float3::Dot(normAxis, obbA->zAxis_) * obbA->size_.z);
	// Bの半径投影
	float rB = fabsf(Float3::Dot(normAxis, obbB->xAxis_) * obbB->size_.x) + fabsf(Float3::Dot(normAxis, obbB->yAxis_) * obbB->size_.y) + fabsf(Float3::Dot(normAxis, obbB->zAxis_) * obbB->size_.z);

	// 分離軸が存在すればfalse
	return centerDist > (rA + rB);
}
