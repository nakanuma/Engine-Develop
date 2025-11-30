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
	float gTime;     /* 経過時間 */
	float amplitude; /* 振幅 */
	float frequency; /* 周波数 */
	float speed;     /* 速度 */
};

/// <summary>
/// グリッチエフェクト用の定数バッファ構造体
/// </summary>
struct GlitchCBData {
	float gTime;     /* 経過時間 */
	float intensity; /* 強度 */
	float speed;     /* 速度 */
	float padding;   /* パディング */
};

/// <summary>
/// ポストエフェクトの種類を列挙する列挙型
/// </summary>
enum class PostEffectType {
	None,             /* エフェクトなし */
	RadialBlur,       /* 放射状ブラー */
	GrayScale,        /* グレースケール */
	Vignette,         /* ビネット */
	Bloom,            /* ブルーム */
	BoxFilter,        /* ボックスフィルター */
	GaussianFilter,   /* ガウシアンフィルター */
	InvertColor,      /* 色反転 */
	Sepia,            /* セピア調 */
	Posterize,        /* ポスタリゼーション */
	Emboss,           /* エンボス */
	Sharpen,          /* シャープ */
	ColorAberration,  /* 色収差 */
	BarrelDistortion, /* バレルディストーション */
	WaveDistortion,   /* 波形ディストーション */
	Pixelation,       /* ピクセル化 */
	GlitchEffect,     /* グリッチエフェクト */
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
	/// メインシーンのレンダリングを開始します。（全ての3Dオブジェクトをレンダーテクスチャに描画する前に呼ぶ）
	/// </summary>
	void BeginMainScene();

	/// <summary>
	/// メインシーンのレンダリングを終了します。
	/// </summary>
	void EndMainScene();

	/// <summary>
	/// Bloomエフェクト描画用のレンダリングを開始します。
	/// </summary>
	void BeginBloom();

	/// <summary>
	/// Bloomエフェクト描画用のレンダリングを終了します。
	/// </summary>
	void EndBloom();

	/// <summary>
	/// バックバッファへの描画状態を強制的に戻します。（直接バックバッファに描画したい場合に使用）
	/// </summary>
	void RestoreBackBuffer(bool resetPSO);

	/// <summary>
	/// デプスバッファのリソースステートを読み取り状態に戻します。（直接バックバッファへ描画した後に使用）
	/// </summary>
	void RestoreDepthBufferState();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// エフェクトタイプの設定を行います。
	/// </summary>
	/// <param name="type">エフェクトタイプ</param>
	void SetEffectType(PostEffectType type) { effectType_ = type; }

	/// <summary>
	/// 現在のエフェクトタイプを取得します。
	/// </summary>
	/// <returns></returns>
	PostEffectType GetEffectType() const { return effectType_; }

	/// <summary>
	/// メインシーンのレンダーテクスチャを取得します。
	/// </summary>
	/// <returns></returns>
	uint32_t GetRenderTextureHandle() const { return mainSceneRT_; }

public:
	// =========================================================
	// Member Variables
	// =========================================================

	ConstBuffer<WaveCBData> waveCB_;     /* 波形ディストーション用定数バッファ */
	ConstBuffer<GlitchCBData> glitchCB_; /* グリッチエフェクト用定数バッファ */

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// 共通の描画処理を行います。
	/// </summary>
	/// <param name="textureHandle">レンダーテクスチャハンドル</param>
	void DrawFullScreenQuad(uint32_t textureHandle);

	/// <summary>
	/// 指定したPSOでフルスクリーン描画を行います。
	/// </summary>
	/// <param name="pso">パイプラインステート</param>
	/// <param name="textureHandle">レンダーテクスチャハンドル</param>
	void DrawWithPSO(ID3D12PipelineState* pso, uint32_t textureHandle);

	/// <summary>
	/// Bloom抽出を行います。
	/// </summary>
	/// <param name="sourceTexture"></param>
	/// <param name="targetRT"></param>
	void ApplyBloomExtract(uint32_t sourceTexture, uint32_t targetRT);

	/// <summary>
	/// ブラー適用を行います。
	/// </summary>
	/// <param name="sourceTexture"></param>
	/// <param name="targetRT"></param>
	void ApplyBloomBlur(uint32_t sourceTexture, uint32_t targetRT);

	/// <summary>
	/// ブラー適用を行います。
	/// </summary>
	/// <param name="sourceTexture"></param>
	/// <param name="targetRT"></param>
	void ApplyBloomBlurHorizontal(uint32_t sourceTexture, uint32_t targetRT);

	// =========================================================
	// Constants
	// =========================================================
	static constexpr size_t kVertexCount = 4; /* 頂点バッファの要素数 */
	static constexpr size_t kIndexCount = 6;  /* インデックスバッファの要素数 */

	static constexpr uint32_t kDrawIndexedCount = 6; /* 描画するインデックス数 */
	static constexpr uint32_t kInstancedCount = 1;   /* 描画するプリミティブのインスタンス数 */

	/* 頂点座標 */
	static constexpr float kNDCMin = -1.0f;
	static constexpr float kNDCMax = 1.0f;
	static constexpr float kNDCZ = 0.0f;
	static constexpr float kNDCW = 1.0f;
	/* 法線ベクトル */
	static constexpr float kNormalX = 0.0f;
	static constexpr float kNormalY = 0.0f;
	static constexpr float kNormalZ = -1.0f;
	/* UV座標 */
	static constexpr float kUVMin = 0.0f;
	static constexpr float kUVMax = 1.0f;
	/* インデックス */
	static constexpr uint32_t kIndex0 = 0;
	static constexpr uint32_t kIndex1 = 1;
	static constexpr uint32_t kIndex2 = 2;
	static constexpr uint32_t kIndex3 = 3;

	static constexpr Float4 kDefaultMaterialColor = {1.0f, 1.0f, 1.0f, 1.0f};
	static constexpr Float4 kBlackMaterialColor = {0.0f, 0.0f, 0.0f, 1.0f};
	static constexpr Float4 kTransparentClearColor = {0.0f, 0.0f, 0.0f, 0.0f};

	/* WaveDistortionの初期値 */
	static constexpr float kWaveTimeInitial = 0.0f;
	static constexpr float kWaveAmplitudeInitial = 0.02f;
	static constexpr float kWaveFrequencyInitial = 10.0f;
	static constexpr float kWaveSpeedInitial = 1.5f;

	/* GlitchEffectの初期値 */
	static constexpr float kGlitchTimeInitial = 0.0f;
	static constexpr float kGlitchIntensityInitial = 1.0f;
	static constexpr float kGlitchSpeedInitial = 0.5f;

	static constexpr uint32_t kRootParameterIndexMaterial = 0;  /* マテリアル用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexTransform = 1; /* 変換行列用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexTexture = 2;   /* テクスチャ用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexWave = 9;      /* WaveDistortionCBV用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexGlitch = 10;   /* GlitchEffectCBV用ルートパラメーターインデックス */

	// =========================================================
	// Member Variables
	// =========================================================
	PostEffectType effectType_ = PostEffectType::None;

	// 頂点・インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbView_;
	D3D12_INDEX_BUFFER_VIEW ibView_;

	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> transformCB_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialCB_;
	Object3D::TransformationMatrix* transformMap_ = nullptr;
	Object3D::Material* materialMap_ = nullptr;

	ConstBuffer<Object3D::Material> blackMaterial_;

	bool initialized_ = false;            // 初期化済みフラグ
	bool isRenderingToOffscreen_ = false; // オフスクリーン描画中フラグ

public:
	// レンダーターゲット
	uint32_t mainSceneRT_ = 0;			/* メインシーン描画用テクスチャ */
	uint32_t bloomResultRT_ = 0;		/* ブルーム適用済みテクスチャ */
	uint32_t bloomExtractRT_ = 0;		/* ブルーム抽出結果テクスチャ */
	uint32_t bloomHorizontalRT_ = 0;
	uint32_t bloomBlurRT_ = 0;			/* ブルームブラー結果テクスチャ */
};
