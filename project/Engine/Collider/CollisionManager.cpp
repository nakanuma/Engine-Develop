#define NOMINMAX
#include "CollisionManager.h"

// C++
#include <algorithm>
#include <iostream>
#include <limits>

// Externals
#include <ImguiWrapper.h>

// Engine
#include <Engine/3D/LineDrawer.h>
#include <Engine/Collider/CollisionMath.h>

CollisionManager* CollisionManager::GetInstance() {
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::Register(Collider* collider) {
	if (collider == nullptr) {
		return;
	}

	colliders_.push_back(collider);
}

void CollisionManager::Unregister(Collider* collider) {
	// colliders_から削除
	auto it = std::remove(colliders_.begin(), colliders_.end(), collider);
	if (it != colliders_.end()) {
		colliders_.erase(it, colliders_.end());
	}

	// previousCollisions_からも削除
	for (auto it2 = previousCollisions_.begin(); it2 != previousCollisions_.end();) {
		if (it2->first == collider || it2->second == collider) {
			it2 = previousCollisions_.erase(it2);
		} else {
			it2++;
		}
	}
}

void CollisionManager::Update() {
	// 今フレームで衝突しているコライダーのペアを一時的に保持
	std::set<std::pair<Collider*, Collider*>> currentCollisions;

	for (size_t i = 0; i < colliders_.size(); ++i) {
		// コライダーが無効ならスキップ
		if (!colliders_[i]->IsActive())
			continue;

		for (size_t j = i + 1; j < colliders_.size(); ++j) {
			// コライダーが無効ならスキップ
			if (!colliders_[j]->IsActive())
				continue;

			if (colliders_[i]->CheckCollision(colliders_[j])) {
				// 今フレームで衝突しているペアをセットに追加
				auto pair = std::make_pair(colliders_[i], colliders_[j]);
				currentCollisions.insert(pair);

				// 前フレームのペアに同じペアが無い場合、ここが衝突した瞬間だと判定
				if (!previousCollisions_.count(pair)) {
					// OnCollisionEnter
					colliders_[i]->GetOwner()->OnCollisionEnter(colliders_[j]);
					colliders_[j]->GetOwner()->OnCollisionEnter(colliders_[i]);
				}

				// OnCollision（stay）
				colliders_[i]->GetOwner()->OnCollision(colliders_[j]);
				colliders_[j]->GetOwner()->OnCollision(colliders_[i]);
			}
		}
	}

	// 前フレームで衝突していたペアが今フレームでは衝突していない場合、ここが衝突終了した瞬間だと判定
	for (auto& pair : previousCollisions_) {
		if (!currentCollisions.count(pair)) {
			// OnCollisionExit
			pair.first->GetOwner()->OnCollisionExit(pair.second);
			pair.second->GetOwner()->OnCollisionExit(pair.first);
		}
	}

	previousCollisions_ = currentCollisions;
}

void CollisionManager::Draw() {
	auto drawer = LineDrawer::GetInstance();

	for (auto* collider : colliders_) {
		if (!collider->IsActive())
			continue;

		///
		///	Sphere
		///
		if (collider->GetType() == "Sphere") {
			auto* sphere = static_cast<SphereCollider*>(collider);

			const uint32_t kSubdivision = 8; // 分割数
			const float kLonEvery = (PIf * 2.0f) / kSubdivision;
			const float kLatEvery = PIf / kSubdivision;

			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				float lat = -PIf / 2.0f + kLatEvery * latIndex;

				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					float lon = lonIndex * kLonEvery;

					// 頂点a, b, cを求める
					Float3 a = {
					    cosf(lat) * cosf(lon) * sphere->GetRadius(),
					    sinf(lat) * sphere->GetRadius(),
					    cosf(lat) * sinf(lon) * sphere->GetRadius(),
					};
					Float3 b = {
					    cosf(lat + kLatEvery) * cosf(lon) * sphere->GetRadius(),
					    sinf(lat + kLatEvery) * sphere->GetRadius(),
					    cosf(lat + kLatEvery) * sinf(lon) * sphere->GetRadius(),
					};
					Float3 c = {
					    cosf(lat) * cosf(lon + kLonEvery) * sphere->GetRadius(),
					    sinf(lat) * sphere->GetRadius(),
					    cosf(lat) * sinf(lon + kLonEvery) * sphere->GetRadius(),
					};

					// ワールド座標に移動
					a = a + sphere->GetCenter();
					b = b + sphere->GetCenter();
					c = c + sphere->GetCenter();

					drawer->RegisterLine(a, b, {1.0f, 1.0f, 1.0f, 1.0f});
					drawer->RegisterLine(a, c, {1.0f, 1.0f, 1.0f, 1.0f});
				}
			}

		}
		///
		///	AABB
		///
		else if (collider->GetType() == "AABB") {
			auto* aabb = static_cast<AABBCollider*>(collider);

			// AABBの8頂点から12本の辺を描画
			Float3 min = aabb->GetMin();
			Float3 max = aabb->GetMax();

			Float3 corners[8] = {
			    {min.x, min.y, min.z},
                {max.x, min.y, min.z},
                {min.x, max.y, min.z},
                {max.x, max.y, min.z},
                {min.x, min.y, max.z},
                {max.x, min.y, max.z},
                {min.x, max.y, max.z},
                {max.x, max.y, max.z},
			};

			int edges[12][2] = {
			    {0, 1},
                {1, 3},
                {3, 2},
                {2, 0}, // 前面
			    {4, 5},
                {5, 7},
                {7, 6},
                {6, 4}, // 背面
			    {0, 4},
                {1, 5},
                {2, 6},
                {3, 7}  // 側面
			};

			for (auto& e : edges) {
				drawer->RegisterLine(corners[e[0]], corners[e[1]], {1.0f, 1.0f, 1.0f, 1.0f});
			}
		}
		///
		///	OBB
		///
		else if (collider->GetType() == "OBB") {
			auto* obb = static_cast<OBBCollider*>(collider);

			// OBBのローカル8頂点を計算
			Float3 halfX = obb->GetXAxis() * (obb->GetSize().x * 1.0f);
			Float3 halfY = obb->GetYAxis() * (obb->GetSize().y * 1.0f);
			Float3 halfZ = obb->GetZAxis() * (obb->GetSize().z * 1.0f);

			Float3 corners[8] = {
			    obb->GetCenter() + halfX + halfY + halfZ, // 0
			    obb->GetCenter() - halfX + halfY + halfZ, // 1
			    obb->GetCenter() - halfX - halfY + halfZ, // 2
			    obb->GetCenter() + halfX - halfY + halfZ, // 3
			    obb->GetCenter() + halfX + halfY - halfZ, // 4
			    obb->GetCenter() - halfX + halfY - halfZ, // 5
			    obb->GetCenter() - halfX - halfY - halfZ, // 6
			    obb->GetCenter() + halfX - halfY - halfZ  // 7
			};

			int edges[12][2] = {
			    {0, 1},
                {1, 2},
                {2, 3},
                {3, 0}, // 前面
			    {4, 5},
                {5, 6},
                {6, 7},
                {7, 4}, // 背面
			    {0, 4},
                {1, 5},
                {2, 6},
                {3, 7}  // 側面
			};

			for (auto& e : edges) {
				drawer->RegisterLine(corners[e[0]], corners[e[1]], {1.0f, 1.0f, 1.0f, 1.0f});
			}
		}
	}
}

void CollisionManager::Debug() {
	if (ImGui::Begin("Colliders")) {

		// 有効なコライダーの数を数える
		size_t activeCount = 0;
		for (auto* c : colliders_) {
			if (c && c->IsActive()) {
				++activeCount;
			}
		}
		ImGui::Text("Total Colliders: %zu", activeCount);
		ImGui::Separator();

		for (size_t i = 0; i < colliders_.size(); ++i) {
			Collider* collider = colliders_[i];
			if (!collider)
				continue;
			if (!collider->IsActive())
				continue;

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
						ImGui::Text("Center : (%.2f, %.2f, %.2f)", sphere->GetCenter().x, sphere->GetCenter().y, sphere->GetCenter().z);
						ImGui::Text("Radius : %.2f", sphere->GetRadius());
					}
				}

				// AABBCollider
				if (collider->GetType() == "AABB") {
					if (auto aabb = dynamic_cast<AABBCollider*>(collider)) {
						Float3 center = (aabb->GetMin() + aabb->GetMax()) * 0.5f;
						ImGui::Text("Center : (%.2f, %.2f, %.2f)", center.x, center.y, center.z);
						ImGui::Text("Min : (%.2f, %.2f, %.2f)", aabb->GetMin().x, aabb->GetMin().y, aabb->GetMin().z);
						ImGui::Text("Max : (%.2f, %.2f, %.2f)", aabb->GetMax().x, aabb->GetMax().y, aabb->GetMax().z);
					}
				}

				// OBBCollider
				if (collider->GetType() == "OBB") {
					if (auto obb = dynamic_cast<OBBCollider*>(collider)) {
						Float3 center = obb->GetCenter();
						Float3 size = obb->GetSize();
						Float3 xAxis = obb->GetXAxis();
						Float3 yAxis = obb->GetYAxis();
						Float3 zAxis = obb->GetZAxis();

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

	///
	///	コライダーの描画
	///

	Draw();
}

bool CollisionManager::RayCast(const Float3& origin, const Float3& direction, float maxDistance, RayCastHit* outHit, const std::unordered_set<std::string>& ignoreTags) {
	bool hitAny = false;
	float closestDistance = maxDistance;
	Collider* closestCollider = nullptr;
	Float3 hitPoint{};

	// 全てのコライダーとの判定
	for (auto* collider : colliders_) {
		// vs AABBCollider
		if (collider->GetType() == "AABB") {
			// 無視タグに含まれるコライダーはスキップ
			if (!ignoreTags.empty() && ignoreTags.count(collider->GetTag()) > 0) {
				continue;
			}

			AABBCollider* aabb = static_cast<AABBCollider*>(collider);

			// レイ方向の逆数を計算
			Float3 invDir = {
			    direction.x != 0.0f ? 1.0f / direction.x : std::numeric_limits<float>::infinity(),
			    direction.y != 0.0f ? 1.0f / direction.y : std::numeric_limits<float>::infinity(),
			    direction.z != 0.0f ? 1.0f / direction.z : std::numeric_limits<float>::infinity(),
			};

			Float3 t1 = (aabb->GetMin() - origin) * invDir;
			Float3 t2 = (aabb->GetMax() - origin) * invDir;

			// 全体の交差区間に変換
			Float3 tmin = Float3::Min(t1, t2);
			Float3 tmax = Float3::Max(t1, t2);

			float tNear = std::max({tmin.x, tmin.y, tmin.z}); // レイがAABBに入る時刻
			float tFar = std::min({tmax.x, tmax.y, tmax.z});  // レイがAABBから出る時刻

			// レイとAABBの交差判定
			if (tNear <= tFar && tNear >= 0.0f && tNear < closestDistance) {
				hitAny = true;
				closestDistance = tNear;
				closestCollider = collider;
				hitPoint = origin + direction * tNear;
			}
		}
		// 他の種類のコライダーとの衝突判定
	}

	// outHitに結果を格納
	if (hitAny && outHit) {
		outHit->isHit = true;
		outHit->hitPoint = hitPoint;
		outHit->hitCollider = closestCollider;
	}

	return hitAny;
}

bool CollisionManager::CheckSphereCollisionWithTag(const Float3& center, float radius, const std::unordered_set<std::string>& targetTags) {
	// 疑似SphereColliderを作成
	SphereCollider tempSphere;
	tempSphere.SetCenter(center);
	tempSphere.SetRadius(radius);

	for (auto* collider : colliders_) {
		// 引数で受け取ったタグを持ったコライダー以外は弾く
		if (!targetTags.empty() && targetTags.count(collider->GetTag()) == 0) {
			continue;
		}

		// Sphere vs Sphere（必要になったら）
		if (collider->GetType() == "Sphere") {
			return false;
		}
		// Sphere vs AABB
		else if (collider->GetType() == "AABB") {
			if (CollisionMath::CheckSphereToAABB(&tempSphere, static_cast<AABBCollider*>(collider))) {
				return true;
			}
		}
		// Sphere vs OBB（必要になったら）
		else if (collider->GetType() == "OBB") {
			return false;
		}
	}

	return false;
}
