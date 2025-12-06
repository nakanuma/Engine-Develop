#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <string>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <MyMath.h>

namespace Cygnus {
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

	/// <summary>
	/// コライダーの更新処理を行います。
	/// </summary>
	virtual void Update() = 0;

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

	/// <summary>
	/// 追従対象の位置を設定します。
	/// </summary>
	/// <param name="target">追従対象の位置へのポインタ</param>
	void SetFollowTarget(Float3* target) { followTarget_ = target; }

	/// <summary>
	/// 追従対象の回転を設定します。（OBB用）
	/// </summary>
	/// <param name="rotation">追従対象の回転へのポインタ</param>
	void SetFollowRotation(Float3* rotation) { followRotation_ = rotation; }

protected:
	// =========================================================
	// Member Variables
	// =========================================================

	std::string tag_;					/* コライダー固有タグ */
	ICollisionCallback* owner_;			/* コライダーのオーナー */
	bool isActive_ = true;				/* コライダーの有効化状態 */
	Float3* followTarget_ = nullptr;	/* 追従対象の位置 */
	Float3* followRotation_ = nullptr;	/* 追従対象の回転（OBBでのみ使用） */
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
	/// コライダーの更新処理を行います。
	/// </summary>
	void Update() override;

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// コライダーの種類を取得します。
	/// </summary>
	/// <returns>コライダーの種類（Sphere）</returns>
	std::string GetType() const override { return "Sphere"; }

	/// <summary>
	/// 球の中心座標を取得します。
	/// </summary>
	/// <returns>球の中心位置</returns>
	const Float3& GetCenter() const { return center_; }

	/// <summary>
	/// 球の中心位置を設定します。
	/// </summary>
	/// <param name="center">球の中心位置</param>
	void SetCenter(const Float3& center) { center_ = center; }

	/// <summary>
	/// 球の半径を取得します。
	/// </summary>
	/// <returns>球の半径</returns>
	float GetRadius() const { return radius_; }

	/// <summary>
	/// 球の半径を設定します。
	/// </summary>
	/// <param name="radius">球の半径</param>
	void SetRadius(float radius) { radius_ = radius; }

private:
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
	/// コライダーの更新処理を行います。
	/// </summary>
	void Update() override;

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

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// コライダーの種類を取得します。
	/// </summary>
	/// <returns>コライダーの種類（AABB）</returns>
	std::string GetType() const override { return "AABB"; }

	/// <summary>
	/// AABBの最小座標を設定します。
	/// </summary>
	/// <param name="min">AABBの最小座標</param>
	void SetMin(const Float3& min) { min_ = min; }

	/// <summary>
	/// AABBの最小座標を取得します。
	/// </summary>
	/// <returns>AABBの最小座標</returns>
	const Float3& GetMin() const { return min_; }

	/// <summary>
	/// AABBの最大座標を設定します。
	/// </summary>
	/// <param name="max">AABBの最大座標</param>
	void SetMax(const Float3& max) { max_ = max; }

	/// <summary>
	/// AABBの最大座標を取得します。
	/// </summary>
	/// <returns>AABBの最大座標</returns>
	const Float3& GetMax() const { return max_; }

	/// <summary>
	/// AABBのサイズを設定します。
	/// </summary>
	/// <param name="size">AABBのサイズ</param>
	void SetSize(const Float3& size) { size_ = size; }

	/// <summary>
	/// AABBのサイズを取得します。
	/// </summary>
	/// <returns>AABBのサイズ</returns>
	const Float3& GetSize() const { return size_; }

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kHalfSize = 0.5f;	/* 押し戻しベクトル計算用 */

	static constexpr float kNormalX = 1.0f;
	static constexpr float kNormalY = 1.0f;
	static constexpr float kNormalZ = 1.0f;
	static constexpr float kNormalZero = 0.0f;

	// =========================================================
	// Member Variables
	// =========================================================

	Float3 min_;			/* AABBの最小座標 */
	Float3 max_;			/* AABBの最大座標 */
	Float3 size_;			/* AABBのサイズ */
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
	/// コライダーの更新処理を行います。
	/// </summary>
	void Update() override;

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// コライダーの種類を取得します。
	/// </summary>
	/// <returns>コライダーの種類（OBB）</returns>
	std::string GetType() const override { return "OBB"; }

	/// <summary>
	/// OBBの中心位置を設定します。
	/// </summary>
	/// <param name="center"></param>
	void SetCenter(const Float3& center) { center_ = center; }

	/// <summary>
	/// OBBの中心位置を取得します。
	/// </summary>
	/// <returns></returns>
	const Float3& GetCenter() const { return center_; }

	/// <summary>
	/// OBBのサイズを設定します。
	/// </summary>
	/// <param name="size">AABBのサイズ</param>
	void SetSize(const Float3& size) { size_ = size; }

	/// <summary>
	/// AABBのサイズを取得します。
	/// </summary>
	/// <returns>AABBのサイズ</returns>
	const Float3& GetSize() const { return size_; }

	/// <summary>
	/// OBBのX軸方向ベクトルを設定します。
	/// </summary>
	/// <param name="xAxis">OBBのX軸方向ベクトル</param>
	void SetXAxis(const Float3& xAxis) { xAxis_ = xAxis; }

	/// <summary>
	/// OBBのX軸方向ベクトルを取得します。
	/// </summary>
	/// <returns>OBBのX軸方向ベクトル</returns>
	const Float3& GetXAxis() const { return xAxis_; }

	/// <summary>
	/// OBBのY軸方向ベクトルを設定します。
	/// </summary>
	/// <param name="yAxis">OBBのY軸方向ベクトル</param>
	void SetYAxis(const Float3& yAxis) { yAxis_ = yAxis; }

	/// <summary>
	/// OBBのY軸方向ベクトルを取得します。
	/// </summary>
	/// <returns>OBBのY軸方向ベクトル</returns>
	const Float3& GetYAxis() const { return yAxis_; }

	/// <summary>
	/// OBBのZ軸方向ベクトルを設定します。
	/// </summary>
	/// <param name="zAxis">OBBのZ軸方向ベクトル</param>
	void SetZAxis(const Float3& zAxis) { zAxis_ = zAxis; }

	/// <summary>
	/// OBBのZ軸方向ベクトルを取得します。
	/// </summary>
	/// <returns>OBBのZ軸方向ベクトル</returns>
	const Float3& GetZAxis() const { return zAxis_; }

private:
	// =========================================================
	// Member Variables
	// =========================================================

	Float3 center_;				/* OBBの中心座標 */
	Float3 size_;				/* OBBの各辺の長さ（幅・高さ・奥行き） */

	Float3 xAxis_;				/* OBBのX軸方向ベクトル */
	Float3 yAxis_;				/* OBBのY軸方向ベクトル */
	Float3 zAxis_;				/* OBBのZ軸方向ベクトル */
};
}