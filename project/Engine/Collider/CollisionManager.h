#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Collider/Collider.h>

/// <summary>
/// レイキャスト結果を保持する構造体
/// </summary>
struct RayCastHit {
	bool isHit = false;						/* 衝突フラグ */
	Float3 hitPoint;						/* 衝突位置 */
	Collider* hitCollider = nullptr;		/* 衝突したコライダー */
};

// =========================================================
// 全てのコライダーを管理するクラス
// =========================================================
class CollisionManager {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static CollisionManager* GetInstance();

	/// <summary>
	/// コライダーを登録します。
	/// </summary>
	/// <param name="collider">コライダー</param>
	void Register(Collider* collider);

	/// <summary>
	/// コライダーの登録を解除します。
	/// </summary>
	/// <param name="collider">コライダー</param>
	void Unregister(Collider* collider);

	/// <summary>
	/// 毎フレームの更新処理（全てのコライダーの衝突判定）を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// コライダーの描画処理（デバッグ用）を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// コライダーのリストをクリアします。
	/// </summary>
	void Clear() {
		colliders_.clear();
		previousCollisions_.clear();
	}

	/// <summary>
	/// デバッグ用の描画処理を行います。
	/// </summary>
	void Debug();

	/// <summary>
	/// レイキャストを行います。
	/// </summary>
	/// <param name="origin">レイの起点</param>
	/// <param name="direction">レイの方向</param>
	/// <param name="maxDistance">最大距離</param>
	/// <param name="outHit">ヒット情報</param>
	/// <param name="ignoreTags">無視するタグのセット</param>
	/// <returns>ヒットした場合はtrue</returns>
	bool RayCast(const Float3& origin, const Float3& direction, float maxDistance, RayCastHit* outHit, const std::unordered_set<std::string>& ignoreTags = {});

	/// <summary>
	/// Sphereと特定タグを持ったコライダーとの衝突判定を行います。
	/// </summary>
	/// <param name="center">Sphereの中心</param>
	/// <param name="radius">Sphereの半径</param>
	/// <param name="targetTags">衝突判定を行うタグのセット</param>
	/// <returns>衝突した場合はtrue</returns>
	bool CheckSphereCollisionWithTag(const Float3& center, float radius, const std::unordered_set<std::string>& targetTags);

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr uint32_t kSphereSubdivision = 8;					/* Sphereデバッグ表示時の分割数 */
	static constexpr size_t kCubeEdgesCount = 12;						/* AABB/OBBの辺の数 */
	static constexpr Float4 kDebugDrawColor = {1.0f, 1.0f, 1.0f, 1.0f};	/* デバッグ描画の色（白） */

	static constexpr float kRayCastZeroEpsilon = 0.0f;	/* レイの逆数を計算する際のゼロ判定値 */
	static constexpr float kRayNearMin = 0.0f;			/* tNearが有効であるとみなす最小値 */

	// =========================================================
	// Member Variables
	// =========================================================

	std::vector<Collider*> colliders_;									/* 全てのコライダーのコンテナ */
	std::set<std::pair<Collider*, Collider*>> previousCollisions_;		/* 前フレームの衝突ペア情報 */
};
