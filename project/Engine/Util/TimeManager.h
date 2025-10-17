#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <chrono>

// =========================================================
// フレームごとの経過時間を管理するクラス
// =========================================================
class TimeManager {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static TimeManager* GetInstance();

	/// <summary>
	/// 毎フレームの更新処理を行います。（deltaTimeの計算）
	/// </summary>
	void Update();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// デルタタイムを取得します。
	/// </summary>
	/// <returns>デルタタイム</returns>
	float GetDeltaTime() const;

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	TimeManager() = default;

	using Clock = std::chrono::high_resolution_clock;

	Clock::time_point previousTime_;					/* 前回の時間 */
	float deltaTime_ = 0.0f;							/* デルタタイム */
};