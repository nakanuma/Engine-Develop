#pragma once

// C++
#include <string>
#include <memory>
#include <unordered_map>

// Engine
#include <Engine/ParticleEffect/IParticleEffect.h>

/// <summary>
/// パーティクルエフェクト管理クラス
/// </summary>
class ParticleEffectManager 
{
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	static ParticleEffectManager* GetInstance();

	/// <summary>
	/// 登録
	/// </summary>
	void Register(const std::string& name, std::shared_ptr<IParticleEffect> effect);

	/// <summary>
	/// 発生処理
	/// </summary>
	void Emit(const std::string& name, const Float3& pos, uint32_t count, const Float3& velocity = {0.0f, 0.0f, 0.0f});

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(float dt);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	std::unordered_map<std::string, std::shared_ptr<IParticleEffect>> effects_;
};
