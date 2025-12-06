#include "Easing.h"

// C++
#include <chrono>
#include <cmath>
#include <numbers>
#include <thread>

float Cygnus::Easing::Lerp(float start, float end, float t) { return start + (end - start) * t; }

float Cygnus::Easing::EaseInSine(float t) { return 1.0f - cosf((t * std::numbers::pi_v<float>) / kTwo); }

float Cygnus::Easing::EaseOutSine(float t) { return sinf((t * std::numbers::pi_v<float>) / kTwo); }

float Cygnus::Easing::EaseInOutSine(float t) { return kHalf * (1.0f - cosf(std::numbers::pi_v<float> * t)); }

float Cygnus::Easing::EaseInCubic(float t) { return t * t * t; }

float Cygnus::Easing::EaseOutCubic(float t) { return (t -= 1.0f) * t * t + 1.0f; }

float Cygnus::Easing::EaseInOutCubic(float t) {
	if (t < 0.5) {
		return kCubicInOutMultiplier * t * t * t;
	} else {
		return (t -= 1.0f) * (t * (t * kCubicInOutMultiplier - 1.0f) + 1.0f);
	}
}

float Cygnus::Easing::EaseInQuint(float t) { return t * t * t * t * t; }

float Cygnus::Easing::EaseOutQuint(float t) { return (t -= 1.0f) * t * t * t * t + 1.0f; }

float Cygnus::Easing::EaseInOutQuint(float t) {
	if (t < 0.5) {
		return kQuintInOutMultiplier * t * t * t * t * t;
	} else {
		return (t -= 1.0f) * t * t * t * t * kQuintInOutMultiplier + 1.0f;
	}
}

float Cygnus::Easing::EaseInCirc(float t) { return 1.0f - sqrtf(1.0f - t * t); }

float Cygnus::Easing::EaseOutCirc(float t) { return sqrtf(1.0f - (t -= 1.0f) * t); }

float Cygnus::Easing::EaseInOutCirc(float t) {
	if (t < kHalf) {
		return kHalf * (1.0f - sqrtf(1.0f - (t * kTwo) * (t * kTwo)));
	} else {
		return kHalf * (sqrtf(1.0f - (t = t * kTwo - kTwo) * t) + 1.0f);
	}
}

float Cygnus::Easing::EaseInElastic(float t) {
	return (t == 0.0f) ? 0.0f
	       : (t == 1.0f)
	           ? 1.0f
	           : -(kElasticA * powf(kTwo, kElasticInOutShift * (t - 1.0f)) * sin((t - 1.0f - (kElasticP / (kTwo * std::numbers::pi_v<float>)) * logf(kElasticA)) * (kTwo * std::numbers::pi_v<float>) / kElasticP));
}

float Cygnus::Easing::EaseOutElastic(float t) {
	return (t == 0.0f)   ? 0.0f
	       : (t == 1.0f) ? 1.0f
	                     : kElasticA * powf(kTwo, -kElasticInOutShift * t) * sinf((t - (kElasticP / (kTwo * std::numbers::pi_v<float>)) * logf(kElasticA)) * (kTwo * std::numbers::pi_v<float>) / kElasticP) + 1.0f;
}

float Cygnus::Easing::EaseInOutElastic(float t) {
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

float Cygnus::Easing::EaseInQuad(float t) { return t * t; }

float Cygnus::Easing::EaseOutQuad(float t) { return 1.0f - (1.0f - t) * (1.0f - t); }

float Cygnus::Easing::EaseInOutQuad(float t) {
	if (t < kHalf) {
		return kTwo * t * t;
	} else {
		return 1.0f - powf(-kTwo * t + kTwo, kTwo) / kTwo;
	}
}

float Cygnus::Easing::EaseInQuart(float t) { return t * t * t * t; }

float Cygnus::Easing::EaseOutQuart(float t) { return 1.0f - powf(1.0f - t, kQuartExponent); }

float Cygnus::Easing::EaseInOutQuart(float t) {
	if (t < kHalf) {
		return kQuartInOutMultiplier * t * t * t * t;
	} else {
		return 1.0f - powf(-kTwo * t + kTwo, kQuartExponent) / kTwo;
	}
}

float Cygnus::Easing::EaseInExpo(float t) { return (t == 0.0f) ? 0.0f : powf(kTwo, kExpoShift * (t - 1.0f)); }

float Cygnus::Easing::EaseOutExpo(float t) { return (t == 1.0f) ? 1.0f : 1.0f - powf(kTwo, -kExpoShift * t); }

float Cygnus::Easing::EaseInOutExpo(float t) {
	if (t == 0.0f || t == 1.0f)
		return t;
	if (t < kHalf)
		return kHalf * pow(kTwo, kExpoInOutMultiplier * t - kExpoInOutShift);
	return kHalf * (kTwo - pow(kTwo, -kExpoInOutMultiplier * t + kExpoInOutShift));
}

float Cygnus::Easing::EaseInBack(float t) { return t * t * ((kBackS + 1.0f) * t - kBackS); }

float Cygnus::Easing::EaseOutBack(float t) { return (t -= 1.0f) * t * ((kBackS + 1.0f) * t + kBackS) + 1.0f; }

float Cygnus::Easing::EaseInOutBack(float t) {
	if (t < kHalf) {
		return kHalf * (t * t * ((kBackS * kBackSInOutMultiplier + 1.0f) * t - kBackS * kBackSInOutMultiplier));
	} else {
		return kHalf * ((t -= 1.0f) * t * ((kBackS * kBackSInOutMultiplier + 1.0f) * t + kBackS * kBackSInOutMultiplier) + kTwo);
	}
}

float Cygnus::Easing::EaseInBounce(float t) { return EaseOutBounce(1.0f - t); }

float Cygnus::Easing::EaseOutBounce(float t) {
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

float Cygnus::Easing::EaseInOutBounce(float t) {
	if (t < kHalf) {
		return kHalf * EaseInBounce(t * kTwo);
	} else {
		return kHalf * EaseOutBounce(t * kTwo - 1.0f) + kHalf;
	}
}