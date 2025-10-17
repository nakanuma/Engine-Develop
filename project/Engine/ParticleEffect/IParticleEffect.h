#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/Math/MyMath.h>

// =========================================================
// パーティクル共通インターフェース
// =========================================================
class IParticleEffect {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IParticleEffect() = default;

	/// <summary>
	/// パーティクルの発生処理を行います。
	/// </summary>
	/// <param name="pos">位置</param>
	/// <param name="velocity">速度ベクトル</param>
	/// <param name="angle">角度</param>
	virtual void Emit(const Float3& pos, const Float3& velocity, const float& angle) = 0;

	/// <summary>
	/// パーティクルの発生処理を行います。
	/// </summary>
	/// <param name="pos">位置</param>
	/// <param name="count">発生させるパーティクルの数</param>
	/// <param name="velocity">速度ベクトル</param>
	/// <param name="angle">角度</param>
	virtual void Emit(const Float3& pos, uint32_t count, const Float3& velocity, const float& angle) {
		for (uint32_t i = 0; i < count; ++i) {
			Emit(pos, velocity, angle);
		}
	}

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="deltaTime"></param>
	virtual void Update(float deltaTime) = 0;

	/// <summary>
	/// パーティクルの描画処理を行います。
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// パーティクルのクリア処理を行います。
	/// </summary>
	virtual void Clear() = 0;
};
