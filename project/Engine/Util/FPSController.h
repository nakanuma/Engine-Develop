#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <chrono>

namespace Cygnus {
// =========================================================
// FPS固定制御クラス
// =========================================================
class FPSController {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static FPSController* GetInstance();

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	void InitializeFixFPS();

	/// <summary>
	/// FPS固定更新処理を行います。
	/// </summary>
	void UpdateFixFPS();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kTargetFPS = 60.0f;					/* 目標FPS */
	static constexpr float kMicrosecondsInSecond = 1000000.0f;	/* 1秒を1マイクロ秒で表した値 */
	static constexpr float kCheckFPS = 65.0f;					/* チェック用のFPS（kTargetFPSよりわずかに速い時間） */
	static constexpr uint32_t kSleepDurationMicroseconds = 1;	/* スリープ時間（マイクロ秒） */

	// =========================================================
	// Member Variables
	// =========================================================
	std::chrono::steady_clock::time_point reference_;		/* 基準時間 */
};
}