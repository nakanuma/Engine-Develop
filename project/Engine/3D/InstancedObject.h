#pragma once
#include "Object3D.h"
#include "StructuredBuffer.h"

/// <summary>
/// インスタンシング描画用の3Dオブジェクトクラス
/// </summary>
class InstancedObject : public Object3D {
public:
	InstancedObject();

	/// <summary>
	/// 描画
	/// </summary>
	void InstancedDraw();

	// 空っぽのStructuredBufferを用意しておく
	StructuredBuffer<Object3D::InstancingObject> gTransformationMatrices = StructuredBuffer<Object3D::InstancingObject>(0, true);
};