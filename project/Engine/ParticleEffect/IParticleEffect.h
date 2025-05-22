#pragma once

// Engine
#include <Engine/Math/MyMath.h>

/// <summary>
/// パーティクル共通インターフェース
/// </summary>
class IParticleEffect {
public:
	virtual ~IParticleEffect() = default;
	
	virtual void Emit(const Float3& pos) = 0;
	virtual void Emit(const Float3& pos, uint32_t count) { 
		for (uint32_t i = 0; i < count; ++i) {
			Emit(pos);
		}
	}
	virtual void Update(float deltaTime) = 0;
	virtual void Draw() = 0;
};
