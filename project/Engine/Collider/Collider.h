#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <string>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <MyMath.h>

// ---------------------------------------------------------
// Foward Declaration
// ---------------------------------------------------------
class Collider;

// =========================================================
// 衝突コールバックインターフェース
// =========================================================
class ICollisionCallback {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~ICollisionCallback() = default;

	/// <summary>
	/// 衝突時コールバック
	/// </summary>
	/// <param name="other">衝突した相手のコライダー</param>
	virtual void OnCollision(Collider* other) {};

	/// <summary>
	/// 衝突開始時コールバック
	/// </summary>
	/// <param name="other">衝突した相手のコライダー</param>
	virtual void OnCollisionEnter(Collider* other) {};

	/// <summary>
	/// 衝突終了時コールバック
	/// </summary>
	/// <param name="other">衝突した相手のコライダー</param>
	virtual void OnCollisionExit(Collider* other) {};
};

// =========================================================
// コライダー基底クラス
// =========================================================
class Collider {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Collider() = default;

	/// <summary>
	/// 衝突判定を行います。
	/// </summary>
	/// <param name="other">衝突相手のコライダー</param>
	/// <returns>衝突している場合はtrue</returns>
	virtual bool CheckCollision(Collider* other) = 0;

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// コライダーの種類を取得します。
	/// </summary>
	/// <returns>コライダーの種類</returns>
	virtual std::string GetType() const = 0;

	/// <summary>
	/// コライダーのタグを設定します。
	/// </summary>
	/// <param name="tag">タグ（string）</param>
	void SetTag(const std::string& tag) { tag_ = tag; }

	/// <summary>
	/// コライダーのタグを取得します。
	/// </summary>
	/// <returns></returns>
	const std::string& GetTag() const { return tag_; }

	/// <summary>
	/// コライダーのオーナーを設定します。
	/// </summary>
	/// <param name="owner">オーナー</param>
	void SetOwner(ICollisionCallback* owner) { owner_ = owner; }

	/// <summary>
	/// コライダーのオーナーを取得します。
	/// </summary>
	/// <returns>オーナー</returns>
	ICollisionCallback* GetOwner() { return owner_; }

	/// <summary>
	/// コライダーの有効化状態を設定します。
	/// </summary>
	/// <param name="active">有効化状態フラグ</param>
	void SetActive(bool active) { isActive_ = active; }

	/// <summary>
	/// コライダーの有効化状態を取得します。
	/// </summary>
	/// <returns>有効化状態</returns>
	bool IsActive() const { return isActive_; }

protected:
	// =========================================================
	// Member Variables
	// =========================================================

	std::string tag_;					/* コライダー固有タグ */
	ICollisionCallback* owner_;			/* コライダーのオーナー */
	bool isActive_ = true;				/* コライダーの有効化状態 */
};

// =========================================================
// 球体コライダー
// =========================================================
class SphereCollider : public Collider {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 衝突判定を行います。
	/// </summary>
	/// <param name="other">衝突相手のコライダー</param>
	/// <returns>衝突している場合はtrue</returns>
	bool CheckCollision(Collider* other) override;

	/// <summary>
	/// コライダーの種類を取得します。
	/// </summary>
	/// <returns>コライダーの種類（Sphere）</returns>
	std::string GetType() const override { return "Sphere"; }

	// =========================================================
	// Member Variables
	// =========================================================

	Float3 center_;			/* 球の中心座標 */
	float radius_;			/* 球の半径 */
};

// =========================================================
// AABBコライダー
// =========================================================
class AABBCollider : public Collider {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 衝突判定を行います。
	/// </summary>
	/// <param name="other">衝突相手のコライダー</param>
	/// <returns>衝突している場合はtrue</returns>
	bool CheckCollision(Collider* other) override;

	/// <summary>
	/// コライダーの種類を取得します。
	/// </summary>
	/// <returns>コライダーの種類（AABB）</returns>
	std::string GetType() const override { return "AABB"; }

	// =========================================================
	// Member Variables
	// =========================================================

	Float3 min_;			/* AABBの最小座標 */
	Float3 max_;			/* AABBの最大座標 */

public:
	/// <summary>
	/// AABB同士の最小押し戻しベクトルを取得します。
	/// </summary>
	/// <param name="other">衝突相手のAABBコライダー</param>
	/// <returns>最小押し戻しベクトル</returns>
	Float3 GetPushBackVector(const AABBCollider& other) const;

	/// <summary>
	/// SphereとAABBの接触面の法線を取得します。
	/// </summary>
	/// <param name="sphereCenter">球の中心座標</param>
	/// <returns>接触面の法線ベクトル</returns>
	Float3 GetContactNormalFromSphere(const Float3& sphereCenter) const;
};

// =========================================================
// OBBコライダー
// =========================================================
class OBBCollider : public Collider {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 衝突判定を行います。
	/// </summary>
	/// <param name="other">衝突相手のコライダー</param>
	/// <returns>衝突している場合はtrue</returns>
	bool CheckCollision(Collider* other) override;

	/// <summary>
	/// コライダーの種類を取得します。
	/// </summary>
	/// <returns>コライダーの種類（OBB）</returns>
	std::string GetType() const override { return "OBB"; }

	// =========================================================
	// Member Variables
	// =========================================================

	Float3 center_;				/* OBBの中心座標 */
	Float3 size_;				/* OBBの各辺の長さ（幅・高さ・奥行き） */
	Float3 xAxis_;				/* OBBのX軸方向ベクトル */
	Float3 yAxis_;				/* OBBのY軸方向ベクトル */
	Float3 zAxis_;				/* OBBのZ軸方向ベクトル */
};