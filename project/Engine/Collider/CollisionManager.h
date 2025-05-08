#pragma once

// C++
#include <vector>

// Engine
#include <Collider/Collider.h>

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
	/// デバッグ表示
	/// </summary>
	void Debug();

private:
	// コライダーのコンテナ
	std::vector<Collider*> colliders_;
};

