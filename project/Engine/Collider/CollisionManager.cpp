#include "CollisionManager.h"

// C++
#include <iostream>

// Externals
#include <ImguiWrapper.h>

CollisionManager* CollisionManager::GetInstance()
{
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::Register(Collider* collider)
{
	if (collider == nullptr) {
		return;
	}

	colliders_.push_back(collider);
}

void CollisionManager::Unregister(Collider* collider)
{
	auto it = std::remove(colliders_.begin(), colliders_.end(), collider);
	if (it != colliders_.end()) {
		colliders_.erase(it, colliders_.end());
	}
}

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
