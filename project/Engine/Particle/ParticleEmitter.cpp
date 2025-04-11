#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(ParticleManager& manager)
{
	this->particleManager = &manager;

	// パーティクルグループをコピー
	for (auto& pair : manager.particleGroups) {
		this->particleGroups[pair.first] = std::move(pair.second);
	}

	// emitterの初期値を設定
	count = 3;
	frequency = 0.5f; // 0.5秒ごとに発生
	frequencyTime = 0.0f; // 発生頻度用の時刻、0で初期化
	transform.translate = { 0.0f, 0.0f, 0.0f };
	transform.rotate = { 0.0f, 0.0f, 0.0f };
	transform.scale = { 1.0f, 1.0f, 1.0f };
}

void ParticleEmitter::Update(std::string name, bool isEmit)
{
	frequencyTime += kDeltaTime; // 時刻を進める
	if (frequency <= frequencyTime) { // 発生時刻より大きいなら発生
		if (isEmit) {
			Emit(name); // 発生させる
		}
		frequencyTime -= frequency; // 余計に過ぎた時間も加味して頻度計算する
	}
}

void ParticleEmitter::Emit(std::string name)
{
	particleManager->Emit(name, transform.translate, count);
}
