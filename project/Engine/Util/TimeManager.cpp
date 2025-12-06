#include "TimeManager.h"

Cygnus::TimeManager* Cygnus::TimeManager::GetInstance() {
	static TimeManager instance;
	return &instance;
}

void Cygnus::TimeManager::Update() {
	// 現在時刻を取得
	auto currentTime = Clock::now();

	if (previousTime_.time_since_epoch().count() != 0) {
		// 前フレームとの時間差を取得
		std::chrono::duration<float> delta = currentTime - previousTime_;
		// 秒単位に変換
		deltaTime_ = delta.count();
	}

	// 現在時刻を保存
	previousTime_ = currentTime;
}

float Cygnus::TimeManager::GetDeltaTime() const { return deltaTime_; }
