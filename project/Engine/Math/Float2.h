#pragma once

/// <summary>
/// 2次元ベクトル
/// </summary>
struct Float2 {
	float x;
	float y;

	Float2 operator+(const Float2& other) const;
	Float2 operator-(const Float2& other) const;

	Float2& operator+=(const Float2& other);

	// ドット積
	static float Dot(const Float2& a, const Float2& b);
};