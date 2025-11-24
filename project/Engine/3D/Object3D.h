#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <ConstBuffer.h>
#include <ModelManager.h>
#include <StructuredBuffer.h>
#include <TextureManager.h>
#include <Transform.h>
#include <Model/Animation/SkinCluster.h>

// =========================================================
// 3Dオブジェクトクラス
// =========================================================
class Object3D {
public:
	/// <summary>
	/// マテリアルを表す構造体
	/// </summary>
	struct Material {
		Float4 color;							/* 色 */
		int32_t enableLighting;					/* ライティング有効フラグ */
		int32_t useEnvironmentMap;				/* 環境マップ使用フラグ */
		float padding[2];						/* パディング */
		Matrix uvTransform;						/* UV変換行列 */
		float shininess;						/* 光沢 */
		float environmentStrength;				/* 環境マップの影響度 */
		float padding2[2];						/* パディング */
		Float3 emissiveColor;					/* 発光色 */
		float emissiveIntensity;				/* 発光強度 */
	};

	/// <summary>
	/// 通常描画用の座標変換行列を表す構造体
	/// </summary>
	struct TransformationMatrix {
		Matrix WVP;								/* ワールドビュー射影行列 */
		Matrix World;							/* ワールド行列 */
		Matrix WorldInverseTranspose;			/* ワールド逆転置行列 */
	};

	/// <summary>
	/// シャドウマッピング用の定数バッファ構造体
	/// </summary>
	struct ShadowCB {
		Matrix World;							/* ワールド行列 */
		Matrix LightViewProj;					/* ライトビュー射影行列 */
	};

	/// <summary>
	/// パーティクル描画用のGPUデータ構造体
	/// </summary>
	struct ParticleForGPU {
		Matrix WVP;								/* ワールドビュー射影行列 */
		Matrix World;							/* ワールド行列 */
		Float4 color;							/* 色 */
	};

	/// <summary>
	/// インスタンシング描画用の定数バッファ構造体
	/// </summary>
	struct InstancingObject {
		Matrix WVP;								/* ワールドビュー射影行列 */
		Matrix World;							/* ワールド行列 */
		Matrix WorldInverseTranspose;			/* ワールド逆転置行列 */
		Float4 color;							/* 色 */
	};

public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Object3D();

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void UpdateMatrix();

	/// <summary>
	/// シャドウマッピング用の行列を更新します。
	/// </summary>
	void UpdateShadowMatrix();

	/// <summary>
	/// UVスケーリングを行います。
	/// </summary>
	/// <param name="scaleU"></param>
	void ScaleUV(float scaleU);

	/// <summary>
	/// 親オブジェクトを設定します。
	/// </summary>
	/// <param name="parent">親オブジェクトのポインタ</param>
	void SetParent(Object3D* parent) { parent_ = parent; }

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// スキンアニメーションの描画処理を行います。
	/// </summary>
	/// <param name="skinCluster">スキンクラスター</param>
	void Draw(SkinCluster skinCluster);

	/// <summary>
	/// インスタンシング描画を行います。
	/// </summary>
	/// <param name="structuredBuffer">構造化バッファ</param>
	/// <param name="numInstance">インスタンス数</param>
	/// <param name="TextureHandle">テクスチャハンドル</param>
	void DrawInstancing(StructuredBuffer<ParticleForGPU>& structuredBuffer, uint32_t numInstance, const uint32_t TextureHandle);

	/// <summary>
	/// 部分的に描画を行います。
	/// </summary>
	/// <param name="indexCount">描画するインデックス数</param>
	void DrawPartial(uint32_t indexCount);

	/// <summary>
	/// シャドウマップ描画を行います。
	/// </summary>
	void DrawShadow();

	/// <summary>
	/// スキンアニメーションのシャドウマップ描画を行います。
	/// </summary>
	/// <param name="skinCluster">スキンクラスター</param>
	void DrawShadow(SkinCluster skinCluster);

	/// <summary>
	/// エミッシブ（発光）を設定します。
	/// </summary>
	/// <param name="color"></param>
	/// <param name="intensity"></param>
	/// <param name="radius"></param>
	/// <param name="decay"></param>
	void SetEmissive(const Float3& color, float intensity, float radius = 5.0f, float decay = 2.0f);

	/// <summary>
	/// エミッシブライトとしてLightManagerに登録します。
	/// </summary>
	void UpdateEmissiveLight();

	/// <summary>
	/// エミッシブを無効化します。
	/// </summary>
	void DisableEmissive();

	// =========================================================
	// Member Variables
	// =========================================================

	ConstBuffer<Material> materialCB_;				/* マテリアルの定数バッファ */

	ConstBuffer<TransformationMatrix> wvpCB_;		/* ワールドビュー射影行列の定数バッファ */
	ConstBuffer<ShadowCB> shadowWvpCB_;				/* シャドウマッピング用の定数バッファ */

	ModelManager::ModelData* model_ = nullptr;		/* モデルデータ */

	Transform transform_;							/* 変換行列 */

private:
	Object3D* parent_ = nullptr;					/* 親オブジェクトへのポインタ */

	bool isEmissive_ = false;		/* 発光フラグ */
	float emissiveRadius_ = 5.0f;	/* 影響範囲 */
	float emissiveDecay_ = 2.0f;	/* 減衰率 */

	// =========================================================
	// Constants
	// =========================================================
	static constexpr uint32_t kRootParameterIndexMaterial = 0;			/* マテリアル用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexWVP = 1;				/* WVP用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexTexture = 2;			/* テクスチャ用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexSkinPaletteSRV = 5;	/* スキンアニメーションSRV用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexShadowCBV = 11;		/* シャドウマッピングWVP用ルートパラメーターインデックス */

	static constexpr uint32_t kMeshVBVStartSlot = 0;	/* メッシュ頂点バッファの開始スロット */
	static constexpr uint32_t kMeshVBVCount = 1;		/* VBVの数 */

	static constexpr uint32_t kSkinMeshVBVCount = 2;	/* スキンメッシュ時のVBV数 */

	static constexpr Float3 kDefaultTranslate = { 0.0f, 0.0f, 0.0f };	/* デフォルトの位置 */
	static constexpr Float3 kDefaultRotation = { 0.0f, 0.0f, 0.0f };	/* デフォルトの回転 */
	static constexpr Float3 kDefaultScale = { 1.0f, 1.0f, 1.0f };		/* デフォルトのスケール */
	static constexpr Float4 kDefaultColor = { 1.0f, 1.0f, 1.0f, 1.0f };	/* デフォルトカラー */
	static constexpr float kDefaultShinniness = 50.0f;					/* デフォルトの光沢 */
	static constexpr float kDefaultEnvironmentStrength = 1.0f;			/* デフォルトの環境反射強度 */

	static constexpr float kDefaultUVScale = 1.0f;	/* UVスケーリングを行わないスケール */

	static constexpr Float3 kDefaultEmissiveColor = {0.0f, 0.0f, 0.0f};	/* デフォルトの発光色 */
	static constexpr float kDefaultEmissiveIntensity = 0.0f;			/* デフォルトの発光時輝度 */
	static constexpr float kDefaultEmissiveRadius = 5.0f;				/* デフォルトの発光時影響範囲 */
	static constexpr float kDefaultEmissiveDecay = 2.0f;				/* デフォルトの発光時減衰率 */
};
