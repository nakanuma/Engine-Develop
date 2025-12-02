#pragma once

// =========================================================
// 2次元ベクトル
// =========================================================
struct Float2 {
	float x;
	float y;

	Float2 operator+(const Float2& other) const;
	Float2 operator-(const Float2& other) const;
	Float2 operator*(float scalar) const;

	Float2& operator+=(const Float2& other);

	/// <summary>
	/// ドット積を計算します。
	/// </summary>
	/// <param name="a">Float2</param>
	/// <param name="b">Float2</param>
	/// <returns>ドット積（Float2）</returns>
	static float Dot(Float2 a, Float2 b);

	/// <summary>
	/// 線形補間を行います。
	/// </summary>
	/// <param name="a">開始点</param>
	/// <param name="b">終了点</param>
	/// <param name="t">補間係数（0.0f から 1.0f の範囲）</param>
	/// <returns>補間結果（Float2）</returns>
	static Float2 Lerp(Float2 a, Float2 b, float t);
};
