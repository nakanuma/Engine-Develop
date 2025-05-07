#pragma once

// C++
#include <chrono>

/// <summary>
/// FPS固定制御
/// </summary>
class FPSController {
public:
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static FPSController* GetInstance();

	/// <summary>
	/// FPS固定初期化
	/// </summary>
	void InitializeFixFPS();

	/// <summary>
	/// FPS固定更新
	/// </summary>
	void UpdateFixFPS();

private:
	// 記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;
};
