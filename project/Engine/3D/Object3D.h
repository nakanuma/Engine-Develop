#pragma once
#include "ConstBuffer.h"
#include "ModelManager.h"
#include "StructuredBuffer.h"
#include "TextureManager.h"
#include "Transform.h"
#include <Model/Animation/SkinCluster.h>

/// <summary>
/// 3Dオブジェクトクラス
/// </summary>
class Object3D {
public:
	/// <summary>
	/// マテリアルを表す構造体
	/// </summary>
	struct Material {
		Float4 color;
		int32_t enableLighting;
		int32_t useEnvironmentMap;
		float padding[2];
		Matrix uvTransform;
		float shininess;
		float environmentStrength;
		float padding2[2];
	};

	/// <summary>
	/// 通常描画用の座標変換行列を表す構造体
	/// </summary>
	struct TransformationMatrix {
		Matrix WVP;
		Matrix World;
		Matrix WorldInverseTranspose;
	};

	/// <summary>
	/// シャドウマッピング用の定数バッファ構造体
	/// </summary>
	struct ShadowCB {
		Matrix World;
		Matrix LightViewProj;
	};

	/// <summary>
	/// パーティクル描画用のGPUデータ構造体
	/// </summary>
	struct ParticleForGPU {
		Matrix WVP;
		Matrix World;
		Float4 color;
	};

	/// <summary>
	/// インスタンシング描画用の定数バッファ構造体
	/// </summary>
	struct InstancingObject {
		Matrix WVP;
		Matrix World;
		Matrix WorldInverseTranspose;
		Float4 color;
	};

	Object3D();

	// マトリックス情報の更新
	void UpdateMatrix();
	void UpdateShadowMatrix();

	// UVスケーリング
	void ScaleUV(float scaleU);

	// 親オブジェクトを設定
	void SetParent(Object3D* parent) { parent_ = parent; }

	// 描画
	void Draw();

	void Draw(SkinCluster skinCluster);

	void DrawInstancing(StructuredBuffer<ParticleForGPU>& structuredBuffer, uint32_t numInstance, const uint32_t TextureHandle);

	// 部分的に描画
	void DrawPartial(uint32_t indexCount);

	// シャドウマップ描画用
	void DrawShadow();

	void DrawShadow(SkinCluster skinCluster);

	// マテリアルの定数バッファ
	ConstBuffer<Material> materialCB_;

	// トランスフォームの定数バッファ
	ConstBuffer<TransformationMatrix> wvpCB_;
	// シャドウマップ用の定数バッファ
	ConstBuffer<ShadowCB> shadowWvpCB_;

	// モデル情報
	ModelManager::ModelData* model_ = nullptr;

	// トランスフォーム情報
	Transform transform_;

private:
	// 親オブジェクトへのポインタ
	Object3D* parent_ = nullptr;
};
