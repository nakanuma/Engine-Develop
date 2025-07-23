#define NOMINMAX
#include "CollisionManager.h"

// C++
#include <iostream>
#include <limits>

// Externals
#include <ImguiWrapper.h>

// ---------------------------------------------------------
// インスタンスの取得
// ---------------------------------------------------------
CollisionManager* CollisionManager::GetInstance()
{
	static CollisionManager instance;
	return &instance;
}

// ---------------------------------------------------------
// コライダーの登録
// ---------------------------------------------------------
void CollisionManager::Register(Collider* collider)
{
	if (collider == nullptr) {
		return;
	}

	colliders_.push_back(collider);
}

// ---------------------------------------------------------
// コライダーの登録を解除
// ---------------------------------------------------------
void CollisionManager::Unregister(Collider* collider)
{
	auto it = std::remove(colliders_.begin(), colliders_.end(), collider);
	if (it != colliders_.end()) {
		colliders_.erase(it, colliders_.end());
	}
}

// ---------------------------------------------------------
// 全ての衝突判定を行う
// ---------------------------------------------------------
void CollisionManager::Update()
{
	for (size_t i = 0; i < colliders_.size(); ++i) {
		for (size_t j = i + 1; j < colliders_.size(); ++j) {
			if (colliders_[i]->CheckCollision(colliders_[j])) {
				colliders_[i]->GetOwner()->OnCollision(colliders_[j]);
				colliders_[j]->GetOwner()->OnCollision(colliders_[i]);
			}
		}
	}
}

// ---------------------------------------------------------
// デバッグ表示
// ---------------------------------------------------------
void CollisionManager::Debug()
{
	if (ImGui::Begin("Colliders")) {
		ImGui::Text("Total Colliders: %zu", colliders_.size());
		ImGui::Separator();

		for (size_t i = 0; i < colliders_.size(); ++i) {
			Collider* collider = colliders_[i];
			if (!collider) continue;

			std::string label = "Collider[" + std::to_string(i) + "] (" + collider->GetTag() + ")";
			if (ImGui::TreeNode(label.c_str())) {
				///
				///	各項目の表示
				/// 

				// タイプ
				ImGui::Text("Type : %s", collider->GetType().c_str());
				
				// タグ
				ImGui::Text("Tag : %s", collider->GetTag().c_str());

				///
				/// パラメーター表示
				/// 
				
				// SphereCollider
				if (collider->GetType() == "Sphere") {
					if (auto sphere = dynamic_cast<SphereCollider*>(collider)) {
						ImGui::Text("Center : (%.2f, %.2f, %.2f)", sphere->center_.x, sphere->center_.y, sphere->center_.z);
						ImGui::Text("Radius : %.2f", sphere->radius_);
					}
				}

				// AABBCollider
				if (collider->GetType() == "AABB") {
					if (auto aabb = dynamic_cast<AABBCollider*>(collider)) {
						Float3 center = (aabb->min_ + aabb->max_) * 0.5f;
						ImGui::Text("Center : (%.2f, %.2f, %.2f)", center.x, center.y, center.z);
						ImGui::Text("Min : (%.2f, %.2f, %.2f)", aabb->min_.x, aabb->min_.y, aabb->min_.z);
						ImGui::Text("Max : (%.2f, %.2f, %.2f)", aabb->max_.x, aabb->max_.y, aabb->max_.z);
					}
				}

				// OBBCollider
				if (collider->GetType() == "OBB") {
					if (auto obb = dynamic_cast<OBBCollider*>(collider)) {
						Float3 center = obb->center_;
						Float3 size = obb->size_;
						Float3 xAxis = obb->xAxis_;
						Float3 yAxis = obb->yAxis_;
						Float3 zAxis = obb->zAxis_;

						ImGui::Text("Center : (%.2f, %.2f, %.2f)", center.x, center.y, center.z);
						ImGui::Text("Size : (%.2f, %.2f, %.2f)", size.x, size.y, size.z);
						ImGui::Text("xAxis : (%.2f, %.2f, %.2f)", xAxis.x, xAxis.y, xAxis.z);
						ImGui::Text("yAxis : (%.2f, %.2f, %.2f)", yAxis.x, yAxis.y, yAxis.z);
						ImGui::Text("zAxis : (%.2f, %.2f, %.2f)", zAxis.x, zAxis.y, zAxis.z);
					}
				}

				ImGui::TreePop();
			}
		}
	}

	ImGui::End();
}

// ---------------------------------------------------------
// レイキャスト
// ---------------------------------------------------------
bool CollisionManager::RayCast(const Float3& origin, const Float3& direction, float maxDistance, RayCastHit* outHit) 
{
	bool hitAny = false;
	float closestDistance = maxDistance;
	Collider* closestCollider = nullptr;
	Float3 hitPoint{};

	// 全てのコライダーとの判定
	for (auto* collider : colliders_) 
	{
		// vs AABBCollider
		if (collider->GetType() == "AABB") 
		{
			AABBCollider* aabb = static_cast<AABBCollider*>(collider);

			// レイ方向の逆数を計算
			Float3 invDir = 
			{
			    direction.x != 0.0f ? 1.0f / direction.x : std::numeric_limits<float>::infinity(),
			    direction.y != 0.0f ? 1.0f / direction.y : std::numeric_limits<float>::infinity(),
			    direction.z != 0.0f ? 1.0f / direction.z : std::numeric_limits<float>::infinity(),
			};

			Float3 t1 = (aabb->min_ - origin) * invDir;
			Float3 t2 = (aabb->max_ - origin) * invDir;

			// 全体の交差区間に変換
			Float3 tmin = Float3::Min(t1, t2);
			Float3 tmax = Float3::Max(t1, t2);

			float tNear = std::max({tmin.x, tmin.y, tmin.z}); // レイがAABBに入る時刻
			float tFar = std::min({tmax.x, tmax.y, tmax.z}); // レイがAABBから出る時刻

			// レイとAABBの交差判定
			if (tNear <= tFar && tNear >= 0.0f && tNear < closestDistance)
			{
				hitAny = true;
				closestDistance = tNear;
				closestCollider = collider;
				hitPoint = origin + direction * tNear;
			}
		}
		// 他の種類のコライダーとの衝突判定

	}

	// outHitに結果を格納
	if (hitAny && outHit) 
	{
		outHit->isHit = true;
		outHit->hitPoint = hitPoint;
		outHit->hitCollider = closestCollider;
	}

	return hitAny;
}
