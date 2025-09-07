#include "ParticleEffectManager.h"

// ---------------------------------------------------------
// シングルトンインスタンスの取得
// ---------------------------------------------------------
ParticleEffectManager* ParticleEffectManager::GetInstance() { 
	static ParticleEffectManager particleEffectManager;
	return &particleEffectManager;
}

// ---------------------------------------------------------
// 登録
// ---------------------------------------------------------
void ParticleEffectManager::Register(const std::string& name, std::shared_ptr<IParticleEffect> effect) 
{ 
	effects_[name] = effect; 
}

// ---------------------------------------------------------
// 発生処理
// ---------------------------------------------------------
void ParticleEffectManager::Emit(const std::string& name, const Float3& pos, uint32_t count, const Float3& velocity, const float& angle)
{
	if (effects_.count(name)) {
		effects_[name]->Emit(pos, count, velocity, angle);
	}
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void ParticleEffectManager::Update(float dt) 
{
	for (auto& [_, effect] : effects_) {
		effect->Update(dt);
	}
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void ParticleEffectManager::Draw() 
{
	for (auto& [_, effect] : effects_) {
		effect->Draw();
	}
}
