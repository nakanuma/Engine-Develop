#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Float3.h>

// =========================================================
// クォータニオンクラス
// =========================================================
class Quaternion {
public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;

	Quaternion operator+(const Quaternion& other) const;
	Quaternion operator-(const Quaternion& other) const;
	Quaternion operator*(float scalar) const;
	friend Quaternion operator*(float scalar, const Quaternion& quat);

	Quaternion& operator+=(const Quaternion& other);
	Quaternion& operator-=(const Quaternion& other);

	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	Quaternion() = default;

	/// <summary>
	/// コンストラクタ（各要素を指定して初期化）
	/// </summary>
	Quaternion(float x, float y, float z, float w) : w(w), x(x), y(y), z(z) {}

	/// <summary>
	/// 任意軸回転を表すQuaternionを生成します。
	/// </summary>
	/// <param name="axis">回転軸</param>
	/// <param name="angle">回転角（ラジアン）</param>
	/// <returns>生成されたQuaternion</returns>
	static Quaternion MakeRotateAxisAngleQuaternion(const Float3& axis, float angle);
	
	/// <summary>
	/// 球面線形補間を行います。
	/// </summary>
	/// <param name="a">開始のQuaternion</param>
	/// <param name="b">終了のQuaternion</param>
	/// <param name="t">補間係数（0.0fから1.0fの範囲）</param>
	/// <returns>補間後のQuaternion</returns>
	static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kHalf = 0.5f;
	static constexpr float kSlerpDotThreshold = 0.995f;	/* 線形補間に切り替えるドット積の閾値 */
};
