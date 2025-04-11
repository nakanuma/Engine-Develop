#pragma once
#include "Transform.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "ConstBuffer.h"
#include "StructuredBuffer.h"

class Object3D
{
public:
	struct Material {
		Float4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix uvTransform;
		float shininess;
	};

	struct TransformationMatrix {
		Matrix WVP;
		Matrix World;
		Matrix WorldInverseTranspose;
	};

	struct ParticleForGPU {
		Matrix WVP;
		Matrix World;
		Float4 color;
	};

	Object3D();

	// マトリックス情報の更新
	void UpdateMatrix();

	// 親オブジェクトを設定
	void SetParent(Object3D* parent) { parent_ = parent; }

	// 描画（モデル内のテクスチャを参照 / テクスチャを指定して描画）
	void Draw();

	void Draw(const int TextureHandle);

	void Draw(ModelManager::SkinCluster skinCluster);

	void DrawInstancing(StructuredBuffer<ParticleForGPU>& structuredBuffer, uint32_t numInstance, const uint32_t TextureHandle);

	// マテリアルの定数バッファ
	ConstBuffer<Material>materialCB_;

	// トランスフォームの定数バッファ
	ConstBuffer<TransformationMatrix>wvpCB_;

	// モデル情報
	ModelManager::ModelData* model_ = nullptr;

	// トランスフォーム情報
	Transform transform_;

private:
	// 親オブジェクトへのポインタ
	Object3D* parent_ = nullptr;
};

