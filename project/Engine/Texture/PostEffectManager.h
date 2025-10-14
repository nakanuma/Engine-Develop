#pragma once

// Engine
#include <DirectXBase.h>
#include <Object3D.h>
#include <Sprite.h>

struct WaveCBData {
	float gTime;
	float amplitude;
	float frequency;
	float speed;
};

struct GlitchCBData {
	float gTime;
	float intensity;
	float speed;
	float padding;
};

enum class PostEffectType {
	None,
	RadialBlur,
	GrayScale,
	Vignette,
	BoxFilter,
	GaussianFilter,
	InvertColor,
	Sepia,
	Posterize,
	Emboss,
	Sharpen,
	ColorAberration,
	BarrelDistortion,
	WaveDistortion,
	Pixelation,
	GlitchEffect,
};

/// <summary>
/// ポストエフェクト管理クラス
/// </summary>
class PostEffectManager {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// コンスタントバッファ送信
	/// </summary>
	void TransfarConstantBuffer();

	/// <summary>
	/// 描画開始
	/// </summary>
	void BeginRenderToTexture();

	/// <summary>
	/// ポストエフェクト適用
	/// </summary>
	void ApplyEffect();

	/// <summary>
	/// レンダーテクスチャ取得
	/// </summary>
	/// <returns></returns>
	uint32_t GetRenderTextureHandle() const { return renderTextureHandle_; }

	/// <summary>
	/// ポストエフェクトタイプのセット
	/// </summary>
	void SetEffectType(PostEffectType type) { effectType_ = type; }

	/// <summary>
	/// ポストエフェクトタイプの取得
	/// </summary>
	PostEffectType GetEffectType() const { return effectType_; }

	/// <summary>
	/// アウトライン描画開始
	/// </summary>
	void BeginRenderToOutlineTexture();
	/// <summary>
	/// アウトライン適用
	/// </summary>
	void ApplyOutline();
	/// <summary>
	/// アウトライン描画
	/// </summary>
	void DrawOutline();

	/// <summary>
	/// Bloom適用
	/// </summary>
	void ApplyBloom();
	/// <summary>
	/// Bloom描画
	/// </summary>
	void DrawBloom();

private:
	PostEffectType effectType_ = PostEffectType::RadialBlur;

	uint32_t renderTextureHandle_ = 0;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformCB_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialCB_;

	D3D12_VERTEX_BUFFER_VIEW vbView_;
	D3D12_INDEX_BUFFER_VIEW ibView_;

	Object3D::TransformationMatrix* transformMap_ = nullptr;
	Object3D::Material* materialMap_ = nullptr;

public:
	ConstBuffer<WaveCBData> waveCB_;
	ConstBuffer<GlitchCBData> glitchCB_;

	// Outline
	uint32_t outlineRT_ = 0; // アウトライン適用オブジェクトのみ描画する用
	uint32_t outlineGH_ = 0; // アウトライン適用後のテクスチャ
	ConstBuffer<Sprite::Material> outlineMaterial_;

	// Bloom
	uint32_t bloomExtractGH_ = 0;
	uint32_t bloomBlurGH_ = 0;
};
