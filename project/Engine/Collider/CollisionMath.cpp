#include "CollisionMath.h"

// C++
#include <algorithm>

// Engine
#include <Collider/Collider.h>

bool Cygnus::CollisionMath::CheckSphereToSphere(const SphereCollider* a, const SphereCollider* b) {
	// 2つの球の中心点間の距離を求める
	Float3 diff = a->GetCenter() - b->GetCenter();
	float distSq = Float3::Dot(diff, diff);
	float radiusSum = a->GetRadius() + b->GetRadius();

	// 半径の合計よりも短ければ衝突
	return distSq <= (radiusSum * radiusSum);
}

bool Cygnus::CollisionMath::CheckSphereToAABB(const SphereCollider* sphere, const AABBCollider* aabb) {
	// 最近接点を求める
	Float3 closestPoint{
	    std::clamp(sphere->GetCenter().x, aabb->GetMin().x, aabb->GetMax().x), std::clamp(sphere->GetCenter().y, aabb->GetMin().y, aabb->GetMax().y),
	    std::clamp(sphere->GetCenter().z, aabb->GetMin().z, aabb->GetMax().z)};
	// 最近接点と球の中心との距離を求める
	Float3 diff = sphere->GetCenter() - closestPoint;
	float distSq = Float3::Dot(diff, diff);

	// 距離が半径より小さければ衝突
	return distSq <= (sphere->GetRadius() * sphere->GetRadius());
}

bool Cygnus::CollisionMath::CheckAABBToAABB(const AABBCollider* a, const AABBCollider* b) {
	return (a->GetMin().x <= b->GetMax().x && a->GetMax().x >= b->GetMin().x) && (a->GetMin().y <= b->GetMax().y && a->GetMax().y >= b->GetMin().y) &&
	       (a->GetMin().z <= b->GetMax().z && a->GetMax().z >= b->GetMin().z);
}

bool Cygnus::CollisionMath::CheckAABBToOBB(const AABBCollider* aabb, const OBBCollider* obb) {
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

bool Cygnus::CollisionMath::CheckOBBToOBB(const OBBCollider* a, const OBBCollider* b) {
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

bool Cygnus::CollisionMath::CheckOBBToSphere(const OBBCollider* obb, const SphereCollider* sphere) {
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

bool Cygnus::CollisionMath::IsSeparatedByAxis(const Float3& axis, const OBBCollider* obbA, const OBBCollider* obbB) {
	// 軸がゼロベクトルでないことを確認
	if (Float3::LengthSq(axis) < kAxisZeroEpsilon) {
		return false; // 分離軸ではない
	}

	Float3 normAxis = Float3::Normalize(axis);

	// 投影中心間距離
	float centerDist = fabsf(Float3::Dot(normAxis, obbB->GetCenter() - obbA->GetCenter()));

	// Aの半径投影
	float rA = fabsf(Float3::Dot(normAxis, obbA->GetXAxis()) * obbA->GetSize().x) + fabsf(Float3::Dot(normAxis, obbA->GetYAxis()) * obbA->GetSize().y) +
	           fabsf(Float3::Dot(normAxis, obbA->GetZAxis()) * obbA->GetSize().z);
	// Bの半径投影
	float rB = fabsf(Float3::Dot(normAxis, obbB->GetXAxis()) * obbB->GetSize().x) + fabsf(Float3::Dot(normAxis, obbB->GetYAxis()) * obbB->GetSize().y) +
	           fabsf(Float3::Dot(normAxis, obbB->GetZAxis()) * obbB->GetSize().z);

	// 分離軸が存在すればfalse
	return centerDist > (rA + rB);
}

Cygnus::Float3 Cygnus::CollisionMath::CalculatePushBackOBBvsOBB(const Cygnus::OBBCollider* a, const Cygnus::OBBCollider* b) {
	// 判定すべき全15軸
	std::vector<Float3> axes;
	axes.push_back(a->GetXAxis());
	axes.push_back(a->GetYAxis());
	axes.push_back(a->GetZAxis());
	axes.push_back(b->GetXAxis());
	axes.push_back(b->GetYAxis());
	axes.push_back(b->GetZAxis());

	// 外積軸
	for (int i = 0; i < 3; ++i) {
		Float3 axisA = (i == 0) ? a->GetXAxis() : (i == 1) ? a->GetYAxis() : a->GetZAxis();
		for (int j = 0; j < 3; ++j) {
			Float3 axisB = (j == 0) ? b->GetXAxis() : (j == 1) ? b->GetYAxis() : b->GetZAxis();
			Float3 cross = Float3::Cross(axisA, axisB);
			if (Float3::LengthSq(cross) > 1e-6f) {
				axes.push_back(Float3::Normalize(cross));
			}
		}
	}

	float minOverlap = FLT_MAX;
	Float3 pushAxis = {0, 0, 0};

	for (const auto& axis : axes) {
		// 重なり量を計算
		float rA = fabsf(Float3::Dot(axis, a->GetXAxis()) * a->GetSize().x) + fabsf(Float3::Dot(axis, a->GetYAxis()) * a->GetSize().y) + fabsf(Float3::Dot(axis, a->GetZAxis()) * a->GetSize().z);

		float rB = fabsf(Float3::Dot(axis, b->GetXAxis()) * b->GetSize().x) + fabsf(Float3::Dot(axis, b->GetYAxis()) * b->GetSize().y) + fabsf(Float3::Dot(axis, b->GetZAxis()) * b->GetSize().z);

		float dist = fabsf(Float3::Dot(b->GetCenter() - a->GetCenter(), axis));
		float overlap = (rA + rB) - dist;

		if (overlap <= 0.0f)
			return {0, 0, 0};

		if (overlap < minOverlap) {
			minOverlap = overlap;
			pushAxis = axis;
		}
	}

	// 押し戻す向きの補正（aをbから遠ざける方向)
	Float3 centerDir = a->GetCenter() - b->GetCenter();
	if (Float3::Dot(centerDir, pushAxis) < 0) {
		pushAxis = pushAxis * -1.0f;
	}

	return pushAxis * minOverlap;
}

float Cygnus::CollisionMath::SqDistanceSegmentToPoint(const Cygnus::Float3& a, const Cygnus::Float3& b, const Cygnus::Float3& c) {
	Cygnus::Float3 ab = {b.x - a.x, b.y - a.y, b.z - a.z};
	Cygnus::Float3 ac = {c.x - a.x, c.y - a.y, c.z - a.z};
	Cygnus::Float3 bc = {c.x - b.x, c.y - b.y, c.z - b.z};

	float e = ac.x * ab.x + ac.y * ab.y + ac.z * ab.z;
	if (e <= 0.0f) return ac.x * ac.x + ac.y * ac.y + ac.z * ac.z;

	float f = ab.x * ab.x + ab.y * ab.y + ab.z * ab.z;
	if (e >= f) return bc.x * bc.x + bc.y * bc.y + bc.z * bc.z;

	return (ac.x * ac.x + ac.y * ac.y + ac.z * ac.z) - (e * e) / f;
}

bool Cygnus::CollisionMath::IsSegmentIntersectSphere(const Cygnus::Float3& a, const Cygnus::Float3& b, const Cygnus::Float3& center, float radius) {
	float sqDist = SqDistanceSegmentToPoint(a, b, center);
	return sqDist <= (radius * radius);
}
