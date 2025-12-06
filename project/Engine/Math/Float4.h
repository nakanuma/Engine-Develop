#pragma once

namespace Cygnus {
// ---------------------------------------------------------
// Foward Declaration
// ---------------------------------------------------------
class Matrix;

// =========================================================
// 4次元ベクトル
// =========================================================
struct Float4 {
	float x;
	float y;
	float z;
	float w;

	Float4 operator*(const Matrix& mat);
	Float4& operator/=(float scalar);

	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 4次元ベクトルを行列で変換します。
	/// </summary>
	/// <param name="v">変換する4次元ベクトル</param>
	/// <param name="mat">変換行列</param>
	/// <returns>変換後のベクトル（Float4）</returns>
	static Float4 Transform(const Float4& v, const Matrix& mat);
	
	/// <summary>
	/// 線形補間を行います。
	/// </summary>
	/// <param name="a">開始点</param>
	/// <param name="b">終了点</param>
	/// <param name="t">補間係数（0.0f から 1.0f の範囲）</param>
	/// <returns>補間結果（Float4）</returns>
	static Float4 Lerp(const Float4& a, const Float4& b, float t);
};
}