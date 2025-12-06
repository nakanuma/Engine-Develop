#include "ParticleEffectManager.h"

Cygnus::ParticleEffectManager* Cygnus::ParticleEffectManager::GetInstance() {
	static ParticleEffectManager particleEffectManager;
	return &particleEffectManager;
}

void Cygnus::ParticleEffectManager::Register(const std::string& name, const std::shared_ptr<IParticleEffect>& effect) {
	// 引数の名前をキーにしてマップへ格納
	effects_[name] = std::move(effect); 
}

void Cygnus::ParticleEffectManager::Emit(const std::string& name, const Float3& pos, uint32_t count, const Float3& velocity, float angle) {
	// 登録時の名前があれば発生処理
	if (effects_.count(name)) {
		effects_[name]->Emit(pos, count, velocity, angle);
	}
}

void Cygnus::ParticleEffectManager::Update(float dt) {
	// 全てのパーティクルの更新
	for (auto& [_, effect] : effects_) {
		effect->Update(dt);
	}
}

void Cygnus::ParticleEffectManager::Draw() {
	// 全てのパーティクルの描画
	for (auto& [_, effect] : effects_) {
		effect->Draw();
	}
}

void Cygnus::ParticleEffectManager::Clear() {
	// 全てのパーティクルをクリア
	for (auto& [_, effect] : effects_) {
		effect->Clear();
	}
}