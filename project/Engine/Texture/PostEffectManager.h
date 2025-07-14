#pragma once

// Engine
#include <DirectXBase.h>
#include <Object3D.h>

enum class PostEffectType {
	RadialBlur,
	GrayScale,
	Vignette,
	BoxFilter,
	GaussianFilter,
};

/// <summary>
/// ポストエフェクト管理
/// </summary>
class PostEffectManager
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

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
};

