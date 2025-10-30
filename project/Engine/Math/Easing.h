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
};