#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Float2.h>
#include <Float3.h>
#include <Float4.h>
#include <Quaternion.h>

// =========================================================
// 4x4行列クラス
// =========================================================
class Matrix {
public:
	float r[4][4];

	Matrix operator-() const;

	Matrix operator+(const Matrix& m) const;
	Matrix operator-(const Matrix& m) const;
	Matrix operator*(const Matrix& m) const;

	Matrix& operator+=(const Matrix& m);
	Matrix& operator-=(const Matrix& m);
	Matrix& operator*=(const Matrix& m);

	Float4 operator*(const Float4& vec);

	/// <summary>
	/// コンストラクタ（単位行列で初期化）
	/// </summary>
	Matrix();

	/// <summary>
	/// floatを16個で初期化
	/// </summary>
	Matrix(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33);

	/// <summary>
	/// 単位行列を返します。
	/// </summary>
	/// <returns>単位行列（4x4Matrix）</returns>
	static Matrix Identity();

	/// <summary>
	/// 逆行列を計算します。
	/// </summary>
	/// <param name="m">逆行列を求める行列</param>
	/// <returns>逆行列（4x4Matrix）</returns>
	static Matrix Inverse(Matrix m);

	/// <summary>
	/// 転置行列を計算します。
	/// </summary>
	/// <param name="m">転置行列を求める行列</param>
	/// <returns>転置行列（4x4Matrix）</returns>
	static Matrix Transpose(const Matrix& m);

	/// <summary>
	/// 視野変換行列を計算します。
	/// </summary>
	/// <param name="fov">視野角（ラジアン）</param>
	/// <param name="aspectRatio">アスペクト比</param>
	/// <param name="nearZ">近接クリップ面</param>
	/// <param name="farZ">遠方クリップ面</param>
	/// <returns>視野変換行列（4x4Matrix）</returns>
	static Matrix PerspectiveFovLH(float fov, float aspectRatio, float nearZ, float farZ);

	/// <summary>
	/// 正射影変換行列を計算します。
	/// </summary>
	/// <param name="width">幅</param>
	/// <param name="height">高さ</param>
	/// <param name="nearClip">近接クリップ面</param>
	/// <param name="farClip">遠方クリップ面</param>
	/// <returns>正射影変換行列（4x4Matrix）</returns>
	static Matrix Orthographic(float width, float height, float nearClip, float farClip);

	/// <summary>
	/// オフセンター正射影変換行列を計算します。
	/// </summary>
	/// <param name="left">左クリップ面</param>
	/// <param name="right">右クリップ面</param>
	/// <param name="bottom">下クリップ面</param>
	/// <param name="top">上クリップ面</param>
	/// <param name="nearZ">近接クリップ面</param>
	/// <param name="farZ">遠方クリップ面</param>
	/// <returns>オフセンター正射影変換行列（4x4Matrix）</returns>
	static Matrix OrthographicOffCenterLH(float left, float right, float bottom, float top, float nearZ, float farZ);

	/// <summary>
	/// 視点行列を計算します。
	/// </summary>
	/// <param name="eye">カメラの位置</param>
	/// <param name="target">カメラの注視点</param>
	/// <param name="up">カメラの上方向ベクトル</param>
	/// <returns>視点行列（4x4Matrix）</returns>
	static Matrix LookAtLH(const Float3& eye, const Float3& target, const Float3& up);

	/// <summary>
	/// スケーリング行列を計算します。
	/// </summary>
	/// <param name="scale">スケールベクトル</param>
	/// <returns>スケーリング行列（4x4Matrix）</returns>
	static Matrix Scaling(Float3 scale);

	/// <summary>
	/// 平行移動行列を計算します。
	/// </summary>
	/// <param name="translation">平行移動ベクトル</param>
	/// <returns>平行移動行列（4x4Matrix）</returns>
	static Matrix Translation(Float3 translation);

	/// <summary>
	/// X軸周りの回転行列を計算します。
	/// </summary>
	/// <param name="rad">回転角（ラジアン）</param>
	/// <returns>X軸周りの回転行列（4x4Matrix）</returns>
	static Matrix RotationX(float rad);

	/// <summary>
	/// Y軸周りの回転行列を計算します。
	/// </summary>
	/// <param name="rad">回転角（ラジアン）</param>
	/// <returns>Y軸周りの回転行列（4x4Matrix）</returns>
	static Matrix RotationY(float rad);

	/// <summary>
	/// Z軸周りの回転行列を計算します。
	/// </summary>
	/// <param name="rad">回転角（ラジアン）</param>
	/// <returns>Z軸周りの回転行列（4x4Matrix）</returns>
	static Matrix RotationZ(float rad);

	/// <summary>
	/// オイラー角から回転行列を計算します。
	/// </summary>
	/// <param name="eulerAngles">オイラー角（ラジアン）</param>
	/// <returns>回転行列（4x4Matrix）</returns>
	static Matrix Rotation(const Float3& eulerAngles);

	/// <summary>
	/// ピッチ（X軸周りの回転）行列を計算します。
	/// </summary>
	/// <param name="rad">回転角（ラジアン）</param>
	/// <returns>ピッチ行列（4x4Matrix）</returns>
	static Matrix Pitch(float rad);

	/// <summary>
	/// ヨー（Y軸周りの回転）行列を計算します。
	/// </summary>
	/// <param name="rad">回転角（ラジアン）</param>
	/// <returns>ヨー行列（4x4Matrix）</returns>
	static Matrix Yaw(float rad);

	/// <summary>
	/// ロール（Z軸周りの回転）行列を計算します。
	/// </summary>
	/// <param name="rad">回転角（ラジアン）</param>
	/// <returns>ロール行列（4x4Matrix）</returns>
	static Matrix Roll(float rad);

	/// <summary>
	/// フォワードベクトルとアップベクトルから視点行列を計算します。
	/// </summary>
	/// <param name="forward">フォワードベクトル</param>
	/// <param name="up">アップベクトル</param>
	/// <returns>視点行列（4x4Matrix）</returns>
	static Matrix LookRotation(const Float3& forward, const Float3& up);

	/// <summary>
	/// ロール、ピッチ、ヨーから回転行列を計算します。
	/// </summary>
	/// <param name="roll">ロール角（Z軸周りの回転）</param>
	/// <param name="pitch">ピッチ角（X軸周りの回転）</param>
	/// <param name="yaw">ヨー角（Y軸周りの回転）</param>
	/// <returns>回転行列（4x4Matrix）</returns>
	static Matrix RotationRollPitchYaw(float roll, float pitch, float yaw);

	/// <summary>
	/// クォータニオンから回転行列を計算します。
	/// </summary>
	/// <param name="q">クォータニオン</param>
	/// <returns>回転行列（4x4Matrix）</returns>
	static Matrix QuaternionToRotation(Quaternion q);

	/// <summary>
	/// アフィン変換行列を作成します。
	/// </summary>
	/// <param name="scale">スケールベクトル</param>
	/// <param name="rotate">回転クォータニオン</param>
	/// <param name="translate">平行移動ベクトル</param>
	/// <returns>アフィン変換行列（4x4Matrix）</returns>
	static Matrix MakeAffine(const Float3& scale, const Quaternion& rotate, const Float3 translate);
};

