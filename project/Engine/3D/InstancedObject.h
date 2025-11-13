#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Object3D.h>
#include <StructuredBuffer.h>

// =========================================================
// インスタンシング描画用の3Dオブジェクトクラス
// =========================================================
class InstancedObject : public Object3D {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	InstancedObject();

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	void InstancedDraw();

	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Resources -----
	StructuredBuffer<Object3D::InstancingObject> gTransformationMatrices_ = StructuredBuffer<Object3D::InstancingObject>(0, true); /* 空っぽのStructuredBuffer */
};