#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <functional>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/Math/MyMath.h>

// =========================================================
// イージング関数群
// =========================================================
class Easing {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 線形補間
	/// </summary>
	/// <param name="start">開始時の値</param>
	/// <param name="end">終了時の値</param>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float Lerp(float start, float end, float t);



	/// <summary>
	/// EaseInSine関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInSine(float t);

	/// <summary>
	/// EaseOutSine関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseOutSine(float t);

	/// <summary>
	/// EaseInOutSine関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInOutSine(float t);



	/// <summary>
	/// EaseInCubic関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInCubic(float t);

	/// <summary>
	/// EaseOutCubic関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseOutCubic(float t);

	/// <summary>
	/// EaseInOutCubic関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInOutCubic(float t);



	/// <summary>
	/// EaseInQuint関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInQuint(float t);

	/// <summary>
	/// EaseOutQuint関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseOutQuint(float t);

	/// <summary>
	/// EaseInOutQuint関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInOutQuint(float t);



	/// <summary>
	/// EaseInCirc関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInCirc(float t);

	/// <summary>
	/// EaseOutCirc関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseOutCirc(float t);

	/// <summary>
	/// EaseInOutCirc関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInOutCirc(float t);



	/// <summary>
	/// EaseInElastic関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInElastic(float t);

	/// <summary>
	/// EaseOutElastic関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseOutElastic(float t);

	/// <summary>
	/// EaseInOutElastic関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInOutElastic(float t);



	/// <summary>
	/// EaseInQuad関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInQuad(float t);

	/// <summary>
	/// EaseOutQuad関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseOutQuad(float t);

	/// <summary>
	/// EaseInOutQuad関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInOutQuad(float t);



	/// <summary>
	/// EaseInQuart関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInQuart(float t);

	/// <summary>
	/// EaseOutQuart関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseOutQuart(float t);

	/// <summary>
	/// EaseInOutQuart関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInOutQuart(float t);



	/// <summary>
	/// EaseInExpo関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInExpo(float t);

	/// <summary>
	/// EaseOutExpo関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseOutExpo(float t);

	/// <summary>
	/// EaseInOutExpo関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInOutExpo(float t);



	/// <summary>
	/// EaseInBack関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInBack(float t);

	/// <summary>
	/// EaseOutBack関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseOutBack(float t);

	/// <summary>
	/// EaseInOutBack関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInOutBack(float t);



	/// <summary>
	/// EaseInBounce関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInBounce(float t);

	/// <summary>
	/// EaseOutBounce関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseOutBounce(float t);

	/// <summary>
	/// EaseInOutBounce関数
	/// </summary>
	/// <param name="t">補間係数</param>
	/// <returns>補間結果</returns>
	static float EaseInOutBounce(float t);

private:
	// =========================================================
	// Constants
	// =========================================================

	static constexpr float kHalf = 0.5f;
	static constexpr float kTwo = 2.0f;

	static constexpr float kCubicInOutMultiplier = 4.0f;	/* EaseInOutCubicの乗数 */
	static constexpr float kQuintInOutMultiplier = 16.0f;	/* EaseInOutQuintの乗数 */
	static constexpr float kElasticInOutMultiplier = 20.0f;	/* EaseInOutElasticの乗数 */
	static constexpr float kExpoShift = 10.0f;				/* EaseInExpo/EaseOutExpoの指数 */
	static constexpr float kExpoInOutMultiplier = 20.0f;	/* EaseInOutExpoの指数 */
	static constexpr float kExpoInOutShift = 10.0f;			/* EaseInOutExpoの指数 */
	static constexpr float kQuartExponent = 4.0f;			/* EaseOutQuart/EaseInOutQuartの指数 */
	static constexpr float kQuartInOutMultiplier = 8.0f;	/* EaseInOutQuartの乗数 */

	// Back
	static constexpr float kBackS = 1.70158f;				/* 初期の過剰数 */
	static constexpr float kBackSInOutMultiplier = 1.525f;	/* 調整係数 */

	// Elastic
	static constexpr float kElasticA = 1.0f;				/* 振幅 */
	static constexpr float kElasticP = 0.3f;				/* 周期 */
	static constexpr float kElasticInOutShift = 10.0f;		/* 調整係数 */
	static constexpr float kElasticInOutOffset = 11.125f;	/* 調整係数 */

	// Bounce
	static constexpr float kBounceThreshold1 = 1.0f / 2.75f;	/* 1回目のバウンドのしきい値 */
	static constexpr float kBounceThreshold2 = 2.0f / 2.75f;	/* 2回目のバウンドのしきい値 */
	static constexpr float kBounceThreshold3 = 2.5f / 2.75f;	/* 3回目のバウンドのしきい値 */

	static constexpr float kBounceMultiplier = 7.5625f;			/* 1回目のバウンドの乗数 */
	static constexpr float kBounceSubtract2 = 1.5f / 2.75f;		/* 2回目のバウンドの減算値 */
	static constexpr float kBounceAdd2 = 0.75f;					/* 2回目のバウンドの加算値 */
	static constexpr float kBounceSubtract3 = 2.25f / 2.75f;	/* 3回目のバウンドの減算値 */
	static constexpr float kBounceAdd3 = 0.9375f;				/* 3回目のバウンドの加算値 */
	static constexpr float kBounceSubtract4 = 2.625f / 2.75f;	/* 4回目のバウンドの減算値 */
	static constexpr float kBounceAdd4 = 0.984375f;				/* 4回目のバウンドの加算値 */
};