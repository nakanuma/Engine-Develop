#pragma once

// C++
#include <string>

// Engine
#include <MyMath.h>

class Collider;

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
	std::string tag_;
	ICollisionCallback* owner_;
};

class SphereCollider : public Collider {
public:
	Float3 center_;
	float radius_;

	bool CheckCollision(Collider* other) override;
	std::string GetType() const override { return "Sphere"; }

	bool CheckCollisionWithSphere(SphereCollider* other);
	// OBBなどとの判定は後で追加可能
};