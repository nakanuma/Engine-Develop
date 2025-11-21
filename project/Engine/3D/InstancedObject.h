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
	// Constants
	// =========================================================
	static constexpr uint32_t kRootParameterIndexMaterial = 0;			/* マテリアル用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexWVP = 1;				/* WVP用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexTexture = 2;			/* テクスチャ用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexStructuredBuffer = 5;	/* ストラクチャードバッファ用ルートパラメーターインデックス */

	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Resources -----
	StructuredBuffer<Object3D::InstancingObject> gTransformationMatrices_ = StructuredBuffer<Object3D::InstancingObject>(0, true); /* 空っぽのStructuredBuffer */
};