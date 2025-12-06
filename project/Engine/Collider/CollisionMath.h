#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <MyMath.h>

namespace Cygnus {
// ---------------------------------------------------------
// Foward Declaration
// ---------------------------------------------------------
class SphereCollider;
class AABBCollider;
class OBBCollider;

// =========================================================
// 衝突判定ユーティリティクラス
// =========================================================
class CollisionMath {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// Sphere vs Sphere
	/// </summary>
	/// <param name="a">Sphere1</param>
	/// <param name="b">Sphere2</param>
	/// <returns>衝突していたらtrue</returns>
	static bool CheckSphereToSphere(const SphereCollider* a, const SphereCollider* b);

	/// <summary>
	/// Sphere vs AABB
	/// </summary>
	/// <param name="sphere">Sphere</param>
	/// <param name="aabb">AABB</param>
	/// <returns>衝突していたらtrue</returns>
	static bool CheckSphereToAABB(const SphereCollider* sphere, const AABBCollider* aabb);

	/// <summary>
	/// AABB vs AABB
	/// </summary>
	/// <param name="a">AABB1</param>
	/// <param name="b">AABB2</param>
	/// <returns>衝突していたらtrue</returns>
	static bool CheckAABBToAABB(const AABBCollider* a, const AABBCollider* b);

	/// <summary>
	/// AABB vs OBB
	/// </summary>
	/// <param name="aabb">AABB</param>
	/// <param name="obb">OBB</param>
	/// <returns>衝突していたらtrue</returns>
	static bool CheckAABBToOBB(const AABBCollider* aabb, const OBBCollider* obb);

	/// <summary>
	/// OBB vs OBB
	/// </summary>
	/// <param name="a">OBB1</param>
	/// <param name="b">OBB2</param>
	/// <returns>衝突していたらtrue</returns>
	static bool CheckOBBToOBB(const OBBCollider* a, const OBBCollider* b);

	/// <summary>
	/// OBB vs Sphere
	/// </summary>
	/// <param name="obb">OBB</param>
	/// <param name="sphere">Sphere</param>
	/// <returns>衝突していたらtrue</returns>
	static bool CheckOBBToSphere(const OBBCollider* obb, const SphereCollider* sphere);

	/// <summary>
	/// 分離軸確認用
	/// </summary>
	/// <param name="axis">分離軸</param>
	/// <param name="obbA">OBB1</param>
	/// <param name="obbB">OBB2</param>
	/// <returns>衝突していたらtrue</returns>
	static bool IsSeparatedByAxis(const Float3& axis, const OBBCollider* obbA, const OBBCollider* obbB);

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kAABBCenterScale = 0.5f;				/* AABBをOBBに変換する際の中心座標計算スケール */
	static constexpr Float3 kWorldAxisX = { 1.0f, 0.0f, 0.0f };	/* ワールド軸に平行なOBBのX軸ベクトル成分 */
	static constexpr Float3 kWorldAxisY = { 0.0f, 1.0f, 0.0f };	/* ワールド軸に平行なOBBのY軸ベクトル成分 */
	static constexpr Float3 kWorldAxisZ = { 0.0f, 0.0f, 1.0f };	/* ワールド軸に平行なOBBのZ軸ベクトル成分 */

	static constexpr size_t kSeparatingAxisCount = 15;	/* 分離軸候補の総数（Aの軸3 + Bの軸3 + A*Bの軸9） */
	static constexpr float kAxisZeroEpsilon = 1e-6f;	/* 軸がゼロベクトルでないかを判定する微小な値 */
};
}
