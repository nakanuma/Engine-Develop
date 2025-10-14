#pragma once

// C++
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

// Engine
#include <Collider/Collider.h>

struct RayCastHit {
	bool isHit = false;
	Float3 hitPoint;
	Collider* hitCollider = nullptr;
};

/// <summary>
/// コライダー管理クラス
/// </summary>
class CollisionManager {
public:
	/// <summary>
	/// インスタンスの取得
	/// </summary>
	static CollisionManager* GetInstance();

	/// <summary>
	/// コライダーの登録
	/// </summary>
	void Register(Collider* collider);

	/// <summary>
	/// コライダーの登録を解除
	/// </summary>
	void Unregister(Collider* collider);

	/// <summary>
	/// 全ての衝突判定を行う
	/// </summary>
	void Update();

	/// <summary>
	/// コライダーの描画を行う（デバッグ用）
	/// </summary>
	void Draw();

	/// <summary>
	/// リストのクリア
	/// </summary>
	void Clear() {
		colliders_.clear();
		previousCollisions_.clear();
	}

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void Debug();

	/// <summary>
	/// レイキャスト
	/// </summary>
	bool RayCast(const Float3& origin, const Float3& direction, float maxDistance, RayCastHit* outHit, const std::unordered_set<std::string>& ignoreTags = {});

	/// <summary>
	/// Sphereと特定タグを持ったコライダーとの衝突判定
	/// </summary>
	bool CheckSphereCollisionWithTag(const Float3& center, float radius, const std::unordered_set<std::string>& targetTags);

private:
	// コライダーのコンテナ
	std::vector<Collider*> colliders_;
	// 前フレームの衝突ペアを保存
	std::set<std::pair<Collider*, Collider*>> previousCollisions_;
};
