#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include "Framework.h"

// =========================================================
// ゲーム本体クラス
// =========================================================
class MyGame : public Framework {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;
};
