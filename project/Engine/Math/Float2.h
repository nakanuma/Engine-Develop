#pragma once

// =========================================================
// 2次元ベクトル
// =========================================================
struct Float2 {
	float x;
	float y;

	Float2 operator+(const Float2& other) const;
	Float2 operator-(const Float2& other) const;

	Float2& operator+=(const Float2& other);

	/// <summary>
	/// ドット積を計算します。
	/// </summary>
	/// <param name="a">Float2</param>
	/// <param name="b">Float2</param>
	/// <returns>ドット積（Float2）</returns>
	static float Dot(const Float2& a, const Float2& b);
};
