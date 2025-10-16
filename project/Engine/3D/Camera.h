#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <ConstBuffer.h>
#include <MyMath.h>

/// <summary>
/// カメラ用の定数バッファデータ構造体
/// </summary>
struct CameraCBData {
	Float3 position;/* 現在位置 */
};

// =========================================================
// 3Dカメラ管理クラス
// =========================================================
class Camera {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="translate">位置（Float3）</param>
	/// <param name="rotate">回転（Euler）</param>
	/// <param name="fov">視野角（radian）</param>
	Camera(Float3 translate, Float3 rotate = Float3(0.0f, 0.0f, 0.0f), float fov = PIf / 2.0f);

	/// <summary>
	/// 定数バッファを転送します。
	/// </summary>
	static void TransferConstantBuffer();

	/// <summary>
	/// ビュー行列を生成します。
	/// </summary>
	/// <returns>ビュー行列</returns>
	Matrix MakeViewMatrix();

	/// <summary>
	/// パースペクティブFOV行列を生成します。
	/// </summary>
	/// <returns>パースペクティブFOV行列</returns>
	Matrix MakePerspectiveFovMatrix();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// 現在のカメラを設定します。
	/// </summary>
	/// <param name="camera">3Dカメラ</param>
	static void Set(Camera* camera) { current_ = camera; }

	/// <summary>
	/// 現在のカメラを取得します。
	/// </summary>
	/// <returns>3Dカメラ</returns>
	static Camera* GetCurrent() { return current_; }

	/// <summary>
	/// ビュープロジェクション行列を取得します。
	/// </summary>
	/// <returns>ビュープロジェクション行列</returns>
	Matrix GetViewProjectionMatrix() { return MakeViewMatrix() * MakePerspectiveFovMatrix(); }

public:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Parameters -----
	Transform transform;						/* 変換行列 */
	float fov;									/* 視野角 */
	float nearZ = 0.1f, farZ = 1000.0f;			/* ニアクリップ距離、ファークリップ距離 */

private:
	inline static Camera* current_;				/* 現在のカメラ */
	ConstBuffer<CameraCBData> cameraCB_;		/* カメラ用定数バッファ */
};
