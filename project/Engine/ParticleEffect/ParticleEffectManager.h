#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <memory>
#include <string>
#include <unordered_map>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/ParticleEffect/IParticleEffect.h>

// =========================================================
// パーティクルエフェクト管理クラス
// =========================================================
class ParticleEffectManager {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static ParticleEffectManager* GetInstance();

	/// <summary>
	/// パーティクルエフェクトを登録します。
	/// </summary>
	/// <param name="name">任意の名前（string）</param>
	/// <param name="effect">パーティクルエフェクト</param>
	void Register(const std::string& name, std::shared_ptr<IParticleEffect> effect);

	/// <summary>
	/// パーティクルを発生させます。
	/// </summary>
	/// <param name="name">パーティクルエフェクトの名前</param>
	/// <param name="pos">発生位置</param>
	/// <param name="count">発生させるパーティクルの数</param>
	/// <param name="velocity">発生させるパーティクルの初速度</param>
	/// <param name="angle">発生させるパーティクルの初回転角</param>
	void Emit(const std::string& name, const Float3& pos, uint32_t count, const Float3& velocity = {0.0f, 0.0f, 0.0f}, const float& angle = 0.0f);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="dt">デルタタイム</param>
	void Update(float dt);

	/// <summary>
	/// パーティクルの描画処理を行います。
	/// </summary>
	void Draw();
	
	/// <summary>
	/// パーティクルのクリア処理を行います。
	/// </summary>
	void Clear();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	std::unordered_map<std::string, std::shared_ptr<IParticleEffect>> effects_;			/* パーティクルエフェクトのマップ */
};
