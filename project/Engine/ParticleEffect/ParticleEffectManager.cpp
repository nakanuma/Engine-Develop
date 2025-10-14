#include "ParticleEffectManager.h"

ParticleEffectManager* ParticleEffectManager::GetInstance() {
	static ParticleEffectManager particleEffectManager;
	return &particleEffectManager;
}

void ParticleEffectManager::Register(const std::string& name, std::shared_ptr<IParticleEffect> effect) { effects_[name] = effect; }

void ParticleEffectManager::Emit(const std::string& name, const Float3& pos, uint32_t count, const Float3& velocity, const float& angle) {
	if (effects_.count(name)) {
		effects_[name]->Emit(pos, count, velocity, angle);
	}
}

void ParticleEffectManager::Update(float dt) {
	for (auto& [_, effect] : effects_) {
		effect->Update(dt);
	}
}

void ParticleEffectManager::Draw() {
	for (auto& [_, effect] : effects_) {
		effect->Draw();
	}
}

void ParticleEffectManager::Clear() {
	for (auto& [_, effect] : effects_) {
		effect->Clear();
	}
}