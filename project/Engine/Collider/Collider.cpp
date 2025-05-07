#include "Collider.h"

bool SphereCollider::CheckCollision(Collider* other)
{
    // 衝突したコライダーがSphereだった場合の処理
    if (other->GetType() == "Sphere") {
        return CheckCollisionWithSphere(static_cast<SphereCollider*>(other));
    }

    // 他のコライダーとの衝突処理を追加


    return false;
}

bool SphereCollider::CheckCollisionWithSphere(SphereCollider* other)
{
    float distance = Float3::Length(this->center_ - other->center_);
    float radiusSum = this->radius_ + other->radius_;

    return distance <= radiusSum;
}
