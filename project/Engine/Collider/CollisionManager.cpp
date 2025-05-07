#include "CollisionManager.h"

CollisionManager* CollisionManager::GetInstance()
{
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::Register(Collider* collider)
{
	colliders_.push_back(collider);
}

void CollisionManager::Unregister(Collider* collider)
{
	auto it = std::remove(colliders_.begin(), colliders_.end(), collider);
	if (it != colliders_.end()) {
		colliders_.erase(it, colliders_.end());
	}
}

void CollisionManager::Update()
{
	for (size_t i = 0; i < colliders_.size(); ++i) {
		for (size_t j = i + 1; j < colliders_.size(); ++j) {
			if (colliders_[i]->CheckCollision(colliders_[j])) {
				colliders_[i]->GetOwner()->OnCollision(colliders_[j]);
				colliders_[j]->GetOwner()->OnCollision(colliders_[i]);
			}
		}
	}
}