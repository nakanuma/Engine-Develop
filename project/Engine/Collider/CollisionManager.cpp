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

	for (auto* collider : colliders_) 
	{
		if (collider->GetType() == "AABB") 
		{
			AABBCollider* aabb = static_cast<AABBCollider*>(collider);

			Float3 invDir = 
			{
			    direction.x != 0.0f ? 1.0f / direction.x : std::numeric_limits<float>::infinity(),
			    direction.y != 0.0f ? 1.0f / direction.y : std::numeric_limits<float>::infinity(),
			    direction.z != 0.0f ? 1.0f / direction.z : std::numeric_limits<float>::infinity(),
			};

			Float3 t1 = (aabb->min_ - origin) * invDir;
			Float3 t2 = (aabb->max_ - origin) * invDir;

			Float3 tmin = Float3::Min(t1, t2);
			Float3 tmax = Float3::Max(t1, t2);

			float tNear = std::max({tmin.x, tmin.y, tmin.z});
			float tFar = std::min({tmax.x, tmax.y, tmax.z});

			if (tNear <= tFar && tNear >= 0.0f && tNear < closestDistance)
			{
				hitAny = true;
				closestDistance = tNear;
				closestCollider = collider;
				hitPoint = origin + direction * tNear;
			}
		}
		// 他のコライダーとの衝突判定



	}

	if (hitAny && outHit) 
	{
		outHit->isHit = true;
		outHit->hitPoint = hitPoint;
		outHit->hitCollider = closestCollider;
	}

	return hitAny;
}
