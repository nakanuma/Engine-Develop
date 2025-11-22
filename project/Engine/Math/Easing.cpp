#include "Easing.h"

// C++
#include <chrono>
#include <cmath>
#include <numbers>
#include <thread>

float Easing::Lerp(float start, float end, float t) { return start + (end - start) * t; }

float Easing::EaseInSine(float t) { return 1.0f - cosf((t * std::numbers::pi_v<float>) / kTwo); }

float Easing::EaseOutSine(float t) { return sinf((t * std::numbers::pi_v<float>) / kTwo); }

float Easing::EaseInOutSine(float t) { return kHalf * (1.0f - cosf(std::numbers::pi_v<float> * t)); }

float Easing::EaseInCubic(float t) { return t * t * t; }

float Easing::EaseOutCubic(float t) { return (t -= 1.0f) * t * t + 1.0f; }

float Easing::EaseInOutCubic(float t) {
	if (t < 0.5) {
		return kCubicInOutMultiplier * t * t * t;
	} else {
		return (t -= 1.0f) * (t * (t * kCubicInOutMultiplier - 1.0f) + 1.0f);
	}
}

float Easing::EaseInQuint(float t) { return t * t * t * t * t; }

float Easing::EaseOutQuint(float t) { return (t -= 1.0f) * t * t * t * t + 1.0f; }

float Easing::EaseInOutQuint(float t) {
	if (t < 0.5) {
		return kQuintInOutMultiplier * t * t * t * t * t;
	} else {
		return (t -= 1.0f) * t * t * t * t * kQuintInOutMultiplier + 1.0f;
	}
}

float Easing::EaseInCirc(float t) { return 1.0f - sqrtf(1.0f - t * t); }

float Easing::EaseOutCirc(float t) { return sqrtf(1.0f - (t -= 1.0f) * t); }

float Easing::EaseInOutCirc(float t) {
	if (t < kHalf) {
		return kHalf * (1.0f - sqrtf(1.0f - (t * kTwo) * (t * kTwo)));
	} else {
		return kHalf * (sqrtf(1.0f - (t = t * kTwo - kTwo) * t) + 1.0f);
	}
}

float Easing::EaseInElastic(float t) {
	return (t == 0.0f) ? 0.0f
	       : (t == 1.0f)
	           ? 1.0f
	           : -(kElasticA * powf(kTwo, kElasticInOutShift * (t - 1.0f)) * sin((t - 1.0f - (kElasticP / (kTwo * std::numbers::pi_v<float>)) * logf(kElasticA)) * (kTwo * std::numbers::pi_v<float>) / kElasticP));
}

float Easing::EaseOutElastic(float t) {
	return (t == 0.0f)   ? 0.0f
	       : (t == 1.0f) ? 1.0f
	                     : kElasticA * powf(kTwo, -kElasticInOutShift * t) * sinf((t - (kElasticP / (kTwo * std::numbers::pi_v<float>)) * logf(kElasticA)) * (kTwo * std::numbers::pi_v<float>) / kElasticP) + 1.0f;
}

float Easing::EaseInOutElastic(float t) {
	if (t == 0.0f)
		return 0.0f;
	if (t == 1.0f)
		return 1.0f;

	if (t < kHalf) {
		return -kHalf * (kElasticA * powf(kTwo, kElasticInOutMultiplier * t - kElasticInOutShift) * sinf((kElasticInOutMultiplier * t - kElasticInOutOffset) * (kTwo * std::numbers::pi_v<float>) / kElasticP));
	} else {
		return kElasticA * powf(kTwo, -kElasticInOutMultiplier * t + kElasticInOutShift) * sinf((kElasticInOutMultiplier * t - kElasticInOutOffset) * (kTwo * std::numbers::pi_v<float>) / kElasticP) * kHalf + 1.0f;
	}
}

float Easing::EaseInQuad(float t) { return t * t; }

float Easing::EaseOutQuad(float t) { return 1.0f - (1.0f - t) * (1.0f - t); }

float Easing::EaseInOutQuad(float t) {
	if (t < kHalf) {
		return kTwo * t * t;
	} else {
		return 1.0f - powf(-kTwo * t + kTwo, kTwo) / kTwo;
	}
}

float Easing::EaseInQuart(float t) { return t * t * t * t; }

float Easing::EaseOutQuart(float t) { return 1.0f - powf(1.0f - t, kQuartExponent); }

float Easing::EaseInOutQuart(float t) {
	if (t < kHalf) {
		return kQuartInOutMultiplier * t * t * t * t;
	} else {
		return 1.0f - powf(-kTwo * t + kTwo, kQuartExponent) / kTwo;
	}
}

float Easing::EaseInExpo(float t) { return (t == 0.0f) ? 0.0f : powf(kTwo, kExpoShift * (t - 1.0f)); }

float Easing::EaseOutExpo(float t) { return (t == 1.0f) ? 1.0f : 1.0f - powf(kTwo, -kExpoShift * t); }

float Easing::EaseInOutExpo(float t) {
	if (t == 0.0f || t == 1.0f)
		return t;
	if (t < kHalf)
		return kHalf * pow(kTwo, kExpoInOutMultiplier * t - kExpoInOutShift);
	return kHalf * (kTwo - pow(kTwo, -kExpoInOutMultiplier * t + kExpoInOutShift));
}

float Easing::EaseInBack(float t) { return t * t * ((kBackS + 1.0f) * t - kBackS); }

float Easing::EaseOutBack(float t) { return (t -= 1.0f) * t * ((kBackS + 1.0f) * t + kBackS) + 1.0f; }

float Easing::EaseInOutBack(float t) {
	if (t < kHalf) {
		return kHalf * (t * t * ((kBackS * kBackSInOutMultiplier + 1.0f) * t - kBackS * kBackSInOutMultiplier));
	} else {
		return kHalf * ((t -= 1.0f) * t * ((kBackS * kBackSInOutMultiplier + 1.0f) * t + kBackS * kBackSInOutMultiplier) + kTwo);
	}
}

float Easing::EaseInBounce(float t) { return EaseOutBounce(1.0f - t); }

float Easing::EaseOutBounce(float t) {
	if (t < kBounceThreshold1) {
		return kBounceMultiplier * t * t;
	} else if (t < kBounceThreshold2) {
		t -= kBounceSubtract2;
		return kBounceMultiplier * t * t + kBounceAdd2;
	} else if (t < kBounceThreshold3) {
		t -= kBounceSubtract3;
		return kBounceMultiplier * t * t + kBounceAdd3;
	} else {
		t -= kBounceSubtract4;
		return kBounceMultiplier * t * t + kBounceAdd4;
	}
}

float Easing::EaseInOutBounce(float t) {
	if (t < kHalf) {
		return kHalf * EaseInBounce(t * kTwo);
	} else {
		return kHalf * EaseOutBounce(t * kTwo - 1.0f) + kHalf;
	}
}