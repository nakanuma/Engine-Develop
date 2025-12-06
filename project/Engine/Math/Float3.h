#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <cassert>
#include <cmath>
#include <vector>

namespace Cygnus {
// ---------------------------------------------------------
// Foward Declaration
// ---------------------------------------------------------
class Matrix;

// =========================================================
// 3次元ベクトル
// =========================================================
struct Float3 {
	float x;
	float y;
	float z;

	Float3 operator+(const Float3& other) const;
	Float3 operator-(const Float3& other) const;
	Float3 operator*(const Float3& other) const;

	Float3 operator*(float scalar) const;
	friend Float3 operator*(float scalar, const Float3& vec);

	Float3 operator/(float scalar) const;

	Float3& operator+=(const Float3& other);
	Float3& operator-=(const Float3& other);
	Float3& operator*=(float scalar);

	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// ベクトルの長さを計算します。
	/// </summary>
	/// <param name="v">対象のベクトル</param>
	/// <returns>ベクトルの長さ（Float3）</returns>
	static float Length(const Float3& v);
	
	/// <summary>
	/// ベクトルの長さの2乗を計算します。
	/// </summary>
	/// <param name="v">対象のベクトル</param>
	/// <returns>ベクトルの長さの2乗（float）</returns>
	static float LengthSq(const Float3& v);
	
	/// <summary>
	/// 2つのベクトル間の距離を計算します。
	/// </summary>
	/// <param name="a">ベクトルA</param>
	/// <param name="b">ベクトルB</param>
	/// <returns>距離（Float3）</returns>
	static float Distance(const Float3& a, const Float3& b);
	
	/// <summary>
	/// ベクトルを正規化します。
	/// </summary>
	/// <param name="a">対象のベクトル</param>
	/// <returns>正規化されたベクトル（Float3）</returns>
	static Float3 Normalize(const Float3& a);
	
	/// <summary>
	/// 線形補間を行います。
	/// </summary>
	/// <param name="a">開始点</param>
	/// <param name="b">終了点</param>
	/// <param name="t">補間係数（0.0f から 1.0f の範囲）</param>
	/// <returns>補間結果（Float3）</returns>
	static Float3 Lerp(const Float3& a, const Float3& b, float t);
	
	/// <summary>
	/// ベクトルを行列で変換します。
	/// </summary>
	/// <param name="v">対象のベクトル</param>
	/// <param name="m">変換行列</param>
	/// <returns>変換後のベクトル（Float3）</returns>
	static Float3 Transform(const Float3& v, const Matrix& m);
	
	/// <summary>
	/// ドット積を計算します。
	/// </summary>
	/// <param name="a">ベクトルA</param>
	/// <param name="b">ベクトルB</param>
	/// <returns>ドット積（float）</returns>
	static float Dot(const Float3& a, const Float3& b);
	
	/// <summary>
	/// クロス積を計算します。
	/// </summary>
	/// <param name="a">ベクトルA</param>
	/// <param name="b">ベクトルB</param>
	/// <returns>クロス積（Float3）</returns>
	static Float3 Cross(const Float3& a, const Float3& b);
	
	/// <summary>
	/// 2つのベクトルの最大値を計算します。
	/// </summary>
	/// <param name="a">ベクトルA</param>
	/// <param name="b">ベクトルB</param>
	/// <returns>最大値（Float3）</returns>
	static Float3 Max(const Float3& a, const Float3& b);
	
	/// <summary>
	/// 2つのベクトルの最小値を計算します。
	/// </summary>
	/// <param name="a">ベクトルA</param>
	/// <param name="b">ベクトルB</param>
	/// <returns>最小値（Float3）</returns>
	static Float3 Min(const Float3& a, const Float3& b);

	/// <summary>
	/// CatmullRom補間を行います。
	/// </summary>
	/// <param name="p0">制御点0</param>
	/// <param name="p1">制御点1</param>
	/// <param name="p2">制御点2</param>
	/// <param name="p3">制御点3</param>
	/// <param name="t">補間係数（0.0f から 1.0f の範囲）</param>
	/// <returns>補間結果（Float3）</returns>
	static Float3 CatmullRomInterplation(const Float3& p0, const Float3& p1, const Float3& p2, const Float3& p3, float t);
	
	/// <summary>
	/// CatmullRomスプライン曲線上の座標を得る
	/// </summary>
	/// <param name="points">制御点のリスト</param>
	/// <param name="t">補間係数（0.0f から 1.0f の範囲）</param>
	/// <returns>補間結果（Float3）</returns>
	static Float3 CatmullRomPosition(const std::vector<Float3>& points, float t);

	/// <summary>
	/// 行列からオイラー角を抽出します。
	/// </summary>
	/// <param name="m">変換行列</param>
	/// <returns>オイラー角（Float3）</returns>
	static Float3 MatrixToEulerAngles(const Matrix& m);
	
	/// <summary>
	/// 行列からスケールを抽出します。
	/// </summary>
	/// <param name="m">変換行列</param>
	/// <returns>スケール（Float3）</returns>
	static Float3 ExtractScale(const Matrix& m);
};
}