#pragma once
#include <cassert>
#include <cmath>
#include <vector>

class Matrix;

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Float3 {
	float x;
	float y;
	float z;

	///
	/// 二項演算子のオーバーロード
	///

	// 加算
	Float3 operator+(const Float3& other) const;
	// 減算
	Float3 operator-(const Float3& other) const;
	// 乗算
	Float3 operator*(const Float3& other) const;

	// 右辺のスカラーと乗算
	Float3 operator*(float scalar) const;
	// 左辺のスカラーと乗算
	friend Float3 operator*(float scalar, const Float3& vec);

	///
	/// 複合代入演算子のオーバーロード
	///

	Float3& operator+=(const Float3& other);
	Float3& operator-=(const Float3& other);
	Float3& operator*=(float scalar);

	// 長さ（ノルム）
	static float Length(const Float3& v);
	// 長さの2乗
	static float LengthSq(const Float3& v);
	// 距離
	static float Distance(const Float3& a, const Float3& b);
	// 正規化
	static Float3 Normalize(const Float3& a);
	// 線形補間
	static Float3 Lerp(const Float3& a, const Float3& b, float t);
	// ベクトルの変換
	static Float3 Transform(const Float3& v, const Matrix& m);
	// ドット積
	static float Dot(const Float3& a, const Float3& b);
	// クロス積
	static Float3 Cross(const Float3& a, const Float3& b);
	// 最大値
	static Float3 Max(const Float3& a, const Float3& b);
	// 最小値
	static Float3 Min(const Float3& a, const Float3& b);

	// CatmullRom補間
	static Float3 CatmullRomInterplation(const Float3& p0, const Float3& p1, const Float3& p2, const Float3& p3, float t);
	// CatmullRomスプライン曲線上の座標を得る
	static Float3 CatmullRomPosition(const std::vector<Float3>& points, float t);

	// 行列からオイラー角の抽出
	static Float3 MatrixToEulerAngles(const Matrix& m);
	// 行列からスケールの抽出
	static Float3 ExtractScale(const Matrix& m);
};