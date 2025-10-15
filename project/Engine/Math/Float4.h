#pragma once

class Matrix;

/// <summary>
/// 4次元ベクトル
/// </summary>
struct Float4 {
	float x;
	float y;
	float z;
	float w;

	Float4 operator*(const Matrix& mat);
	Float4& operator/=(float scalar);

	static Float4 Transform(const Float4& v, const Matrix& mat);
	// 線形補間
	static Float4 Lerp(const Float4& a, const Float4& b, float t);
};
