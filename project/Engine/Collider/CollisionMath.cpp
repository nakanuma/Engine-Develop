#include "CollisionMath.h"

// C++
#include <algorithm>

// Engine
#include <Collider/Collider.h>

bool CollisionMath::CheckSphereToSphere(const SphereCollider* a, const SphereCollider* b) {
	// 2つの球の中心点間の距離を求める
	Float3 diff = a->GetCenter() - b->GetCenter();
	float distSq = Float3::Dot(diff, diff);
	float radiusSum = a->GetRadius() + b->GetRadius();

	// 半径の合計よりも短ければ衝突
	return distSq <= (radiusSum * radiusSum);
}

bool CollisionMath::CheckSphereToAABB(const SphereCollider* sphere, const AABBCollider* aabb) {
	// 最近接点を求める
	Float3 closestPoint{
		std::clamp(sphere->GetCenter().x, aabb->GetMin().x, aabb->GetMax().x),
		std::clamp(sphere->GetCenter().y, aabb->GetMin().y, aabb->GetMax().y),
		std::clamp(sphere->GetCenter().z, aabb->GetMin().z, aabb->GetMax().z)
	};
	// 最近接点と球の中心との距離を求める
	Float3 diff = sphere->GetCenter() - closestPoint;
	float distSq = Float3::Dot(diff, diff);

	// 距離が半径より小さければ衝突
	return distSq <= (sphere->GetRadius() * sphere->GetRadius());
}

bool CollisionMath::CheckAABBToAABB(const AABBCollider* a, const AABBCollider* b) {
	return (a->GetMin().x <= b->GetMax().x && a->GetMax().x >= b->GetMin().x) && (a->GetMin().y <= b->GetMax().y && a->GetMax().y >= b->GetMin().y) && (a->GetMin().z <= b->GetMax().z && a->GetMax().z >= b->GetMin().z);
}

bool CollisionMath::CheckAABBToOBB(const AABBCollider* aabb, const OBBCollider* obb) {
	// AABBの中心と半サイズ
	Float3 aabbCenter = (aabb->GetMin() + aabb->GetMax()) * kAABBCenterScale;
	Float3 aabbHalfSize = (aabb->GetMax() - aabb->GetMin()) * kAABBCenterScale;

	// AABBをOBBの形に変換
	OBBCollider aabbAsOBB;
	aabbAsOBB.SetCenter(aabbCenter);
	aabbAsOBB.SetSize(aabbHalfSize);

	// ワールド軸に平行な軸をセット
	aabbAsOBB.SetXAxis(kWorldAxisX);
	aabbAsOBB.SetYAxis(kWorldAxisY);
	aabbAsOBB.SetZAxis(kWorldAxisZ);

	// OBBvsOBBを行う
	return CheckOBBToOBB(&aabbAsOBB, obb);
}

bool CollisionMath::CheckOBBToOBB(const OBBCollider* a, const OBBCollider* b) {
	// 軸候補（分離軸）
	Float3 axis[kSeparatingAxisCount] = {
		// Aのローカル軸
		a->GetXAxis(),
		a->GetYAxis(),
		a->GetZAxis(),
		// Bのローカル軸
		b->GetXAxis(),
		b->GetYAxis(),
		b->GetZAxis(),
		// A x B（外積軸）
		Float3::Cross(a->GetXAxis(), b->GetXAxis()),
		Float3::Cross(a->GetXAxis(), b->GetYAxis()),
		Float3::Cross(a->GetXAxis(), b->GetZAxis()),

		Float3::Cross(a->GetYAxis(), b->GetXAxis()),
		Float3::Cross(a->GetYAxis(), b->GetYAxis()),
		Float3::Cross(a->GetYAxis(), b->GetZAxis()),

		Float3::Cross(a->GetZAxis(), b->GetXAxis()),
		Float3::Cross(a->GetZAxis(), b->GetYAxis()),
		Float3::Cross(a->GetZAxis(), b->GetZAxis()),
	};

	for (size_t i = 0; i < kSeparatingAxisCount; ++i) {
		if (IsSeparatedByAxis(axis[i], a, b)) {
			return false;
		}
	}

	// すべての軸で分離していなければ衝突している
	return true;
}

bool CollisionMath::CheckOBBToSphere(const OBBCollider* obb, const SphereCollider* sphere) {
	// 球の中心をOBB空間に射影
	Float3 d = sphere->GetCenter() - obb->GetCenter();

	// 各軸への投影距離を箱の半サイズ以内で計算
	Float3 closestPoint = obb->GetCenter();

	closestPoint += obb->GetXAxis() * std::clamp(Float3::Dot(d, obb->GetXAxis()), -obb->GetSize().x, obb->GetSize().x);
	closestPoint += obb->GetYAxis() * std::clamp(Float3::Dot(d, obb->GetYAxis()), -obb->GetSize().y, obb->GetSize().y);
	closestPoint += obb->GetZAxis() * std::clamp(Float3::Dot(d, obb->GetZAxis()), -obb->GetSize().z, obb->GetSize().z);

	Float3 vecToSphere = sphere->GetCenter() - closestPoint;
	float distSq = Float3::LengthSq(vecToSphere);

	return distSq <= sphere->GetRadius() * sphere->GetRadius();
}

bool CollisionMath::IsSeparatedByAxis(const Float3& axis, const OBBCollider* obbA, const OBBCollider* obbB) {
	// 軸がゼロベクトルでないことを確認
	if (Float3::LengthSq(axis) < kAxisZeroEpsilon) {
		return false; // 分離軸ではない
	}

	Float3 normAxis = Float3::Normalize(axis);

	// 投影中心間距離
	float centerDist = fabsf(Float3::Dot(normAxis, obbB->GetCenter() - obbA->GetCenter()));

	// Aの半径投影
	float rA =
		fabsf(Float3::Dot(normAxis, obbA->GetXAxis()) * obbA->GetSize().x) +
		fabsf(Float3::Dot(normAxis, obbA->GetYAxis()) * obbA->GetSize().y) +
		fabsf(Float3::Dot(normAxis, obbA->GetZAxis()) * obbA->GetSize().z);
	// Bの半径投影
	float rB =
		fabsf(Float3::Dot(normAxis, obbB->GetXAxis()) * obbB->GetSize().x) +
		fabsf(Float3::Dot(normAxis, obbB->GetYAxis()) * obbB->GetSize().y) +
		fabsf(Float3::Dot(normAxis, obbB->GetZAxis()) * obbB->GetSize().z);

	// 分離軸が存在すればfalse
	return centerDist > (rA + rB);
}
