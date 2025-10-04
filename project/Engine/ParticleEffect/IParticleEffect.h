#pragma once

// Engine
#include <Engine/Math/MyMath.h>

/// <summary>
/// パーティクル共通インターフェース
/// </summary>
class IParticleEffect {
public:
	virtual ~IParticleEffect() = default;
	
	// デフォルト
	virtual void Emit(const Float3& pos, const Float3& velocity, const float& angle) = 0;
	virtual void Emit(const Float3& pos, uint32_t count, const Float3& velocity, const float& angle) {
		for (uint32_t i = 0; i < count; ++i) {
			Emit(pos, velocity, angle);
		}
	}

	virtual void Update(float deltaTime) = 0;
	virtual void Draw() = 0;
	virtual void Clear() = 0;
};
