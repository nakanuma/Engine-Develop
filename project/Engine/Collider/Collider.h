#pragma once

// C++
#include <string>

// Engine
#include <MyMath.h>

class Collider;

/// <summary>
/// 衝突コールバックインターフェース
/// </summary>
class ICollisionCallback {
public:
	virtual ~ICollisionCallback() = default;
	virtual void OnCollision(Collider* other) = 0;
};

/// <summary>
/// コライダー基底クラス
/// </summary>
class Collider
{
public:
	virtual ~Collider() = default;

	virtual bool CheckCollision(Collider* other) = 0;
	virtual std::string GetType() const = 0;

	void SetTag(const std::string& tag) { tag_ = tag; }
	const std::string& GetTag() const { return tag_; }

	void SetOwner(ICollisionCallback* owner) { owner_ = owner; }
	ICollisionCallback* GetOwner() { return owner_; }

protected:
	// コライダー識別用タグ
	std::string tag_;
	// コライダーの所属オブジェクトへのポインタ
	ICollisionCallback* owner_;
};

/// <summary>
/// Sphereコライダー
/// </summary>
class SphereCollider : public Collider {
public:
	Float3 center_;
	float radius_;

	bool CheckCollision(Collider* other) override;
	std::string GetType() const override { return "Sphere"; }
};

/// <summary>
/// AABBコライダー
/// </summary>
class AABBCollider : public Collider {
public:
	Float3 min_;
	Float3 max_;

	bool CheckCollision(Collider* other) override;
	std::string GetType() const override { return "AABB"; }

	/// <summary>
	/// AABB同士の最小押し戻しベクトルを取得
	/// </summary>
	Float3 GetPushBackVector(const AABBCollider& other) const;
};