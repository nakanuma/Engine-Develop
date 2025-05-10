#pragma once

// 前方宣言
class SphereCollider;
class AABBCollider;
class OBBCollider;

/// <summary>
/// 衝突判定処理
/// </summary>
class CollisionMath
{
public:
	/// <summary>
	/// Sphere vs Sphere
	/// </summary>
	static bool CheckSphereToSphere(const SphereCollider* a, const SphereCollider* b);

	/// <summary>
	/// Sphere vs AABB
	/// </summary>
	static bool CheckSphereToAABB(const SphereCollider* sphere, const AABBCollider* aabb);

	/// <summary>
	/// AABB vs AABB
	/// </summary>
	static bool CheckAABBToAABB(const AABBCollider* a, const AABBCollider* b);

	/// <summary>
	/// AABB vs OBB
	/// </summary>
	static bool CheckAABBToOBB(const AABBCollider* aabb, const OBBCollider* obb);

	/// <summary>
	/// OBB vs OBB
	/// </summary>
	static bool CheckOBBToOBB(const OBBCollider* a, const OBBCollider* b);

	/// <summary>
	/// OBB vs Sphere
	/// </summary>
	static bool CheckOBBToSphere(const OBBCollider* obb, const SphereCollider* sphere);
};

