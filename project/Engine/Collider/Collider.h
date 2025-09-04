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
	virtual void OnCollision(Collider* other) {};
	virtual void OnCollisionEnter(Collider* other) {};
	virtual void OnCollisionExit(Collider* other) {};
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

	void SetActive(bool active) { isActive_ = active; }
	bool IsActive() const { return isActive_; }

protected:
	// コライダー識別用タグ
	std::string tag_;
	// コライダーの所属オブジェクトへのポインタ
	ICollisionCallback* owner_;
	// 有効化状態
	bool isActive_ = true;
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

public:
	/// <summary>
	/// AABB同士の最小押し戻しベクトルを取得
	/// </summary>
	Float3 GetPushBackVector(const AABBCollider& other) const;

	/// <summary>
	/// SphereとAABBの接触面の法線を取得
	/// </summary>
	Float3 GetContactNormalFromSphere(const Float3& sphereCenter) const;
};

/// <summary>
/// OBBコライダー
/// </summary>
class OBBCollider : public Collider {
public:
	Float3 center_;
	Float3 size_;
	Float3 xAxis_;
	Float3 yAxis_;
	Float3 zAxis_;

	bool CheckCollision(Collider* other) override;
	std::string GetType() const override { return "OBB"; }
};