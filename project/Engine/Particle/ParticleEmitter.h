#pragma once
#include "ParticleEmitter.h"
#include "ParticleManager.h"

class ParticleEmitter
{
public:
	ParticleEmitter(ParticleManager& manager);

	void Update(std::string name, bool isEmit);
	void Emit(std::string name);
private:
	ParticleManager* particleManager = nullptr;

	// パーティクルグループコンテナ
	std::unordered_map<std::string, std::unique_ptr<ParticleManager::ParticleGroup>> particleGroups;

	// Δtを定義
	const float kDeltaTime = 1.0f / 60.0f;

	Transform transform; //!< エミッタのTransform
	uint32_t count; //!< 発生数
	float frequency; //!< 発生頻度
	float frequencyTime; // !< 頻度用時刻
};

