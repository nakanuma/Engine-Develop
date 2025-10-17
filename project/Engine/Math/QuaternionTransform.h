#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Float3.h>
#include <Matrix.h>
#include <Quaternion.h>

// =========================================================
// スケール・回転（Quaternion）・平行移動をまとめた変換
// =========================================================
class QuaternionTransform {
public:
	Float3 scale;
	Quaternion rotate;
	Float3 translate;

	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// アフィン変換行列を生成します。
	/// </summary>
	/// <returns>生成されたアフィン変換行列</returns>
	Matrix MakeAffineMatrix();
};
