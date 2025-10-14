#include "Easing.h"

// C++
#include <chrono>
#include <cmath>
#include <numbers>
#include <thread>

const float BACK_S = 1.70158f;

const float ELASTIC_A = 1.0f;
const float ELASTIC_P = 0.3f;

float Easing::Lerp(float start, float end, float t) { return start + (end - start) * t; }

float Easing::EaseInSine(float t) { return 1.0f - cosf((t * std::numbers::pi_v<float>) / 2.0f); }

float Easing::EaseOutSine(float t) { return sinf((t * std::numbers::pi_v<float>) / 2.0f); }

float Easing::EaseInOutSine(float t) { return 0.5f * (1.0f - cosf(std::numbers::pi_v<float> * t)); }

float Easing::EaseInCubic(float t) { return t * t * t; }

float Easing::EaseOutCubic(float t) { return (t -= 1.0f) * t * t + 1.0f; }

float Easing::EaseInOutCubic(float t) {
	if (t < 0.5) {
		return 4.0f * t * t * t;
	} else {
		return (t -= 1.0f) * (t * (t * 4.0f - 1.0f) + 1.0f);
	}
}

float Easing::EaseInQuint(float t) { return t * t * t * t * t; }

float Easing::EaseOutQuint(float t) { return (t -= 1.0f) * t * t * t * t + 1.0f; }

float Easing::EaseInOutQuint(float t) {
	if (t < 0.5) {
		return 16.0f * t * t * t * t * t;
	} else {
		return (t -= 1.0f) * t * t * t * t * 16.0f + 1.0f;
	}
}

float Easing::EaseInCirc(float t) { return 1.0f - sqrtf(1.0f - t * t); }

float Easing::EaseOutCirc(float t) { return sqrtf(1.0f - (t -= 1.0f) * t); }

float Easing::EaseInOutCirc(float t) {
	if (t < 0.5f) {
		return 0.5f * (1.0f - sqrtf(1.0f - (t * 2.0f) * (t * 2.0f)));
	} else {
		return 0.5f * (sqrtf(1.0f - (t = t * 2.0f - 2.0f) * t) + 1.0f);
	}
}

float Easing::EaseInElastic(float t) {
	return (t == 0.0f) ? 0.0f
	       : (t == 1.0f)
	           ? 1.0f
	           : -(ELASTIC_A * powf(2.0f, 10.0f * (t - 1.0f)) * sin((t - 1.0f - (ELASTIC_P / (2.0f * std::numbers::pi_v<float>)) * logf(ELASTIC_A)) * (2.0f * std::numbers::pi_v<float>) / ELASTIC_P));
}

float Easing::EaseOutElastic(float t) {
	return (t == 0.0f)   ? 0.0f
	       : (t == 1.0f) ? 1.0f
	                     : ELASTIC_A * powf(2.0f, -10.0f * t) * sinf((t - (ELASTIC_P / (2.0f * std::numbers::pi_v<float>)) * logf(ELASTIC_A)) * (2.0f * std::numbers::pi_v<float>) / ELASTIC_P) + 1.0f;
}

float Easing::EaseInOutElastic(float t) {
	if (t == 0.0f)
		return 0.0f;
	if (t == 1.0f)
		return 1.0f;

	if (t < 0.5f) {
		return -0.5f * (ELASTIC_A * powf(2.0f, 20.0f * t - 10.0f) * sinf((20.0f * t - 11.125f) * (2.0f * std::numbers::pi_v<float>) / ELASTIC_P));
	} else {
		return ELASTIC_A * powf(2.0f, -20.0f * t + 10.0f) * sinf((20.0f * t - 11.125f) * (2.0f * std::numbers::pi_v<float>) / ELASTIC_P) * 0.5f + 1.0f;
	}
}

float Easing::EaseInQuad(float t) { return t * t; }

float Easing::EaseOutQuad(float t) { return 1.0f - (1.0f - t) * (1.0f - t); }

float Easing::EaseInOutQuad(float t) {
	if (t < 0.5f) {
		return 2.0f * t * t;
	} else {
		return 1.0f - powf(-2.0f * t + 2.0f, 2.0f) / 2.0f;
	}
}

float Easing::EaseInQuart(float t) { return t * t * t * t; }

float Easing::EaseOutQuart(float t) { return 1.0f - powf(1.0f - t, 4.0f); }

float Easing::EaseInOutQuart(float t) {
	if (t < 0.5f) {
		return 8.0f * t * t * t * t;
	} else {
		return 1.0f - powf(-2.0f * t + 2.0f, 4.0f) / 2.0f;
	}
}

float Easing::EaseInExpo(float t) { return (t == 0.0f) ? 0.0f : powf(2.0f, 10.0f * (t - 1.0f)); }

float Easing::EaseOutExpo(float t) { return (t == 1.0f) ? 1.0f : 1.0f - powf(2.0f, -10.0f * t); }

float Easing::EaseInOutExpo(float t) {
	if (t == 0.0f || t == 1.0f)
		return t;
	if (t < 0.5f)
		return 0.5f * pow(2.0f, 20.0f * t - 10.0f);
	return 0.5f * (2.0f - pow(2.0f, -20.0f * t + 10.0f));
}

float Easing::EaseInBack(float t) { return t * t * ((BACK_S + 1.0f) * t - BACK_S); }

float Easing::EaseOutBack(float t) { return (t -= 1.0f) * t * ((BACK_S + 1.0f) * t + BACK_S) + 1.0f; }

float Easing::EaseInOutBack(float t) {
	if (t < 0.5f) {
		return 0.5f * (t * t * ((BACK_S * 1.525f + 1.0f) * t - BACK_S * 1.525f));
	} else {
		return 0.5f * ((t -= 1.0f) * t * ((BACK_S * 1.525f + 1.0f) * t + BACK_S * 1.525f) + 2.0f);
	}
}

float Easing::EaseInBounce(float t) { return EaseOutBounce(1.0f - t); }

float Easing::EaseOutBounce(float t) {
	if (t < (1.0f / 2.75f)) {
		return 7.5625f * t * t;
	} else if (t < (2.0f / 2.75f)) {
		t -= (1.5f / 2.75f);
		return 7.5625f * t * t + 0.75f;
	} else if (t < (2.5f / 2.75f)) {
		t -= (2.25f / 2.75f);
		return 7.5625f * t * t + 0.9375f;
	} else {
		t -= (2.625f / 2.75f);
		return 7.5625f * t * t + 0.984375f;
	}
}

float Easing::EaseInOutBounce(float t) {
	if (t < 0.5f) {
		return 0.5f * EaseInBounce(t * 2.0f);
	} else {
		return 0.5f * EaseOutBounce(t * 2.0f - 1.0f) + 0.5f;
	}
}

void SimpleEasing::Animate(float& value, float start, float end, EasingFunction easingFunction, float duration) {
	std::thread([&, start, end, duration, easingFunction]() {
		auto startTime = std::chrono::high_resolution_clock::now();
		// アニメーション終了時刻の計算
		auto endTime = startTime + std::chrono::milliseconds(static_cast<int>(duration * 1000));

		// 現在時刻がアニメーション終了時刻になるまでループ
		while (std::chrono::high_resolution_clock::now() < endTime) {
			auto now = std::chrono::high_resolution_clock::now();
			auto elapsedTime = std::chrono::duration<float>(now - startTime).count();
			float t = elapsedTime / duration;
			// tの値をクランプ
			if (t > 1.0f)
				t = 1.0f;

			// 遷移途中のvalueを計算
			value = start + (end - start) * easingFunction(t);

			std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 約60fpsで更新
		}

		// valueを最終値に設定
		value = end;
	}).detach(); // スレッドを切り離して非同期処理
}

void SimpleEasing::Animate(Float3& value, Float3 start, Float3 end, EasingFunction easingFunction, float duration) {
	std::thread([&, start, end, duration, easingFunction]() {
		auto startTime = std::chrono::high_resolution_clock::now();
		// アニメーション終了時刻の計算
		auto endTime = startTime + std::chrono::milliseconds(static_cast<int>(duration * 1000));

		// 現在時刻がアニメーション終了時刻になるまでループ
		while (std::chrono::high_resolution_clock::now() < endTime) {
			auto now = std::chrono::high_resolution_clock::now();
			auto elapsedTime = std::chrono::duration<float>(now - startTime).count();
			float t = elapsedTime / duration;
			// tの値をクランプ
			if (t > 1.0f)
				t = 1.0f;

			// 遷移途中のvalueを計算
			value = start + (end - start) * easingFunction(t);

			std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 約60fpsで更新
		}

		// valueを最終値に設定
		value = end;
	}).detach(); // スレッドを切り離して非同期処理
}