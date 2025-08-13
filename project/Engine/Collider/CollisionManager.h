#pragma once

// C++
#include <vector>

// Engine
#include <Collider/Collider.h>

struct RayCastHit
{
	bool isHit = false;
	Float3 hitPoint;
	Collider* hitCollider = nullptr;
};

class CollisionManager
{
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
	/// リストのクリア
	/// </summary>
	void Clear() { colliders_.clear(); }

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void Debug();

	/// <summary>
	/// レイキャスト
	/// </summary>
	bool RayCast(const Float3& origin, const Float3& direction, float maxDistance, RayCastHit* outHit);

private:
	// コライダーのコンテナ
	std::vector<Collider*> colliders_;
};

