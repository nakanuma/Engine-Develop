#include "Collider.h"

// Engine
#include <Collider/CollisionMath.h>

bool SphereCollider::CheckCollision(Collider* other) {
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

void SphereCollider::Update() {
	// 位置を追従させる
	if (followTarget_) {
		center_ = *followTarget_;
	}
}

bool AABBCollider::CheckCollision(Collider* other) {
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

void AABBCollider::Update()
{
	// minとmaxの更新
	if (followTarget_) {
		Float3 center = *followTarget_;
		min_ = center - size_;
		max_ = center + size_;
	}
}

Float3 AABBCollider::GetPushBackVector(const AABBCollider& other) const {
	// それぞれの中心座標を取得
	Float3 centerA = (min_ + max_) * 0.5f;
	Float3 centerB = (other.min_ + other.max_) * 0.5f;

	// それぞれの半サイズを取得
	Float3 halfSizeA = (max_ - min_) * 0.5f;
	Float3 halfSizeB = (other.max_ - other.min_) * 0.5f;

	// 中心間ベクトルを計算
	Float3 delta = centerA - centerB;
	// 各軸での重なり量を計算
	Float3 overlap = {
		(halfSizeA.x + halfSizeB.x) - std::abs(delta.x),
		(halfSizeA.y + halfSizeB.y) - std::abs(delta.y),
		(halfSizeA.z + halfSizeB.z) - std::abs(delta.z)
	};


	if (overlap.x < overlap.y && overlap.x < overlap.z) {
		// X軸方向に押し戻すベクトルを返す
		return { (delta.x > 0 ? overlap.x : -overlap.x), 0.0f, 0.0f };
	} else if (overlap.y < overlap.z) {
		// Y軸方向に押し戻すベクトルを返す
		return { 0.0f, (delta.y > 0 ? overlap.y : -overlap.y), 0.0f };
	} else {
		// Z軸方向に押し戻すベクトルを返す
		return { 0.0f, 0.0f, (delta.z > 0 ? overlap.z : -overlap.z) };
	}
}

Float3 AABBCollider::GetContactNormalFromSphere(const Float3& sphereCenter) const {
	Float3 clamped;
	// 球の中心をAABB内部にクランプして最近傍点を求める
	clamped.x = std::max(min_.x, std::min(sphereCenter.x, max_.x));
	clamped.y = std::max(min_.y, std::min(sphereCenter.y, max_.y));
	clamped.z = std::max(min_.z, std::min(sphereCenter.z, max_.z));

	// 球の中心とAABBの最近傍点との差分ベクトル
	Float3 delta = sphereCenter - clamped;

	// 各軸の差分の絶対値を比較して、最も影響が大きい軸を探す
	if (std::abs(delta.x) > std::abs(delta.y) && std::abs(delta.x) > std::abs(delta.z)) {
		// X軸方向から接触
		return { delta.x > 0 ? 1.0f : -1.0f, 0.0f, 0.0f };
	} else if (std::abs(delta.y) > std::abs(delta.z)) {
		// Y軸方向から接触
		return { 0.0f, delta.y > 0 ? 1.0f : -1.0f, 0.0f };
	} else {
		// Z軸方向から接触
		return { 0.0f, 0.0f, delta.z > 0 ? 1.0f : -1.0f };
	}
}

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

void OBBCollider::Update()
{
	if (followTarget_) {
		// 位置を更新
		center_ = *followTarget_;

		if (followRotation_) {
			// 回転行列を作成して、コライダーの回転軸の更新
			Matrix rotMat = Matrix::Rotation(*followRotation_);
			xAxis_ = Float3::Normalize(Float3(rotMat.r[0][0], rotMat.r[1][0], rotMat.r[2][0]));
			yAxis_ = Float3::Normalize(Float3(rotMat.r[0][1], rotMat.r[1][1], rotMat.r[2][1]));
			zAxis_ = Float3::Normalize(Float3(rotMat.r[0][2], rotMat.r[1][2], rotMat.r[2][2]));
		}
	}
}