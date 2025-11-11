#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <DirectXBase.h>
#include <Object3D.h>
#include <Sprite.h>

/// <summary>
/// 波形ディストーション用の定数バッファ構造体
/// </summary>
struct WaveCBData {
	float gTime;				/* 経過時間 */
	float amplitude;			/* 振幅 */
	float frequency;			/* 周波数 */
	float speed;				/* 速度 */
};

/// <summary>
/// グリッチエフェクト用の定数バッファ構造体
/// </summary>
struct GlitchCBData {
	float gTime;				/* 経過時間 */
	float intensity;			/* 強度 */
	float speed;				/* 速度 */
	float padding;				/* パディング */
};

/// <summary>
/// ポストエフェクトの種類を列挙する列挙型
/// </summary>
enum class PostEffectType {
	None,						/* エフェクトなし */
	RadialBlur,					/* 放射状ブラー */
	GrayScale,					/* グレースケール */
	Vignette,					/* ビネット */
	Bloom,						/* ブルーム */
	BoxFilter,					/* ボックスフィルター */
	GaussianFilter,				/* ガウシアンフィルター */
	InvertColor,				/* 色反転 */
	Sepia,						/* セピア調 */
	Posterize,					/* ポスタリゼーション */
	Emboss,						/* エンボス */
	Sharpen,					/* シャープ */
	ColorAberration,			/* 色収差 */
	BarrelDistortion,			/* バレルディストーション */
	WaveDistortion,				/* 波形ディストーション */
	Pixelation,					/* ピクセル化 */
	GlitchEffect,				/* グリッチエフェクト */
};

// =========================================================
// ポストエフェクト管理クラス
// =========================================================
class PostEffectManager {
public:
	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	void Initialize();

	/// <summary>
	/// 定数バッファを転送します。
	/// </summary>
	void TransfarConstantBuffer();

	/// <summary>
	/// レンダリングを開始します。
	/// </summary>
	void BeginRenderToTexture();

	/// <summary>
	/// ポストエフェクトを適用します。
	/// </summary>
	void ApplyEffect();
	
	/// <summary>
	/// レンダーテクスチャのハンドルを取得します。
	/// </summary>
	/// <returns>レンダーテクスチャハンドル</returns>
	uint32_t GetRenderTextureHandle() const { return renderTextureHandle_; }

	/// <summary>
	/// エフェクトタイプを設定します。
	/// </summary>
	/// <param name="type">エフェクトタイプ</param>
	void SetEffectType(PostEffectType type) { effectType_ = type; }

	/// <summary>
	/// エフェクトタイプを取得します。
	/// </summary>
	/// <returns>エフェクトタイプ</returns>
	PostEffectType GetEffectType() const { return effectType_; }

	/// <summary>
	/// アウトライン用のレンダリングを開始します。
	/// </summary>
	void BeginRenderToOutlineTexture();

	/// <summary>
	/// アウトラインエフェクトを適用します。
	/// </summary>
	void ApplyOutline();

	/// <summary>
	/// アウトラインを描画します。
	/// </summary>
	void DrawOutline();

	/// <summary>
	/// ブルームエフェクトを適用します。
	/// </summary>
	void ApplyBloom();
	
	/// <summary>
	/// ブルーム効果を描画します。
	/// </summary>
	void DrawBloom();

public:
	// =========================================================
	// Member Variables
	// =========================================================

	ConstBuffer<WaveCBData> waveCB_;								/* 波形ディストーション用定数バッファ */
	ConstBuffer<GlitchCBData> glitchCB_;							/* グリッチエフェクト用定数バッファ */

	uint32_t outlineRT_ = 0;										/* アウトライン用レンダーテクスチャハンドル */
	uint32_t outlineGH_ = 0;										/* アウトライン用ガウシアンハンドル */
	ConstBuffer<Sprite::Material> outlineMaterial_;					/* アウトライン用マテリアル定数バッファ */

	uint32_t bloomExtractGH_ = 0;									/* ブルーム抽出用ガウシアンハンドル */
	uint32_t bloomBlurGH_ = 0;										/* ブルームブラー用ガウシアンハンドル */

private:
	PostEffectType effectType_ = PostEffectType::RadialBlur;		/* エフェクトタイプ */

	uint32_t renderTextureHandle_ = 0;								/* レンダーテクスチャハンドル */

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;			/* 頂点バッファ */
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer_;			/* インデックスバッファ */
	Microsoft::WRL::ComPtr<ID3D12Resource> transformCB_;			/* 変換行列用定数バッファ */
	Microsoft::WRL::ComPtr<ID3D12Resource> materialCB_;				/* マテリアル用定数バッファ */

	D3D12_VERTEX_BUFFER_VIEW vbView_;								/* 頂点バッファビュー */
	D3D12_INDEX_BUFFER_VIEW ibView_;								/* インデックスバッファビュー */

	Object3D::TransformationMatrix* transformMap_ = nullptr;		/* 変換行列マップ */
	Object3D::Material* materialMap_ = nullptr;						/* マテリアルマップ */

	bool initialized_ = false;										/* 初期化フラグ */
};
