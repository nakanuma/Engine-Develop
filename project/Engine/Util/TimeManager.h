#pragma once

// C++
#include <chrono>

class TimeManager
{
public:
	/// <summary>
	/// インスタンスの取得
	/// </summary>
	static TimeManager* GetInstance();

	/// <summary>
	///  更新処理（deltaTimeの計算）
	/// </summary>
	void Update();

	/// <summary>
	/// DeltaTimeの取得
	/// </summary>
	float GetDeltaTime() const;

private:
	TimeManager() = default;

	using Clock = std::chrono::high_resolution_clock;
	// 前フレーム時刻
	Clock::time_point previousTime_;
	// 前フレームから現在時刻までの経過秒数
	float deltaTime_ = 0.0f;
};
