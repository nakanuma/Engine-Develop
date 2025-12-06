#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <DescriptorHeap.h>
#include <DirectXBase.h>

namespace Cygnus{
// =========================================================
// スプライト描画に関する共通処理をまとめたクラス
// =========================================================
class SpriteCommon {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// スプライト共通処理クラスの初期化を行います。
	/// </summary>
	/// <param name="dxBase">DirectX基盤クラス</param>
	void Initialize(DirectXBase* dxBase);

	/// <summary>
	/// 共通描画設定を行います。
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 共通描画後処理を行います。
	/// </summary>
	void PostDraw();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// DirectX基盤クラスを取得します。
	/// </summary>
	/// <returns>DirectX基盤クラス</returns>
	DirectXBase* GetDxBase() const { return dxBase_; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// ルートシグネチャを生成します。
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// グラフィックスパイプラインを生成します。
	/// </summary>
	void CreateGraphicsPipeline();

	/// <summary>
	/// 入力レイアウトを設定します。
	/// </summary>
	void SetInputLayout();

	/// <summary>
	/// DXC初期化を行います。
	/// </summary>
	void InitializeDXC();

	/// <summary>
	/// ラスタライザステートを設定します。
	/// </summary>
	D3D12_RASTERIZER_DESC SetRasterizerState();

	/// <summary>
	/// 深度バッファを生成します。
	/// </summary>
	void CreateDepthBuffer();

	/// <summary>
	/// ブレンドステートを設定します。
	/// </summary>
	D3D12_BLEND_DESC SetBlendState();

	/// <summary>
	/// ブレンドステート(BlendMode::None用)を設定します。
	/// </summary>
	D3D12_BLEND_DESC SetBlendStateNone();

	/// <summary>
	/// ブレンドステート(BlendMode::Add用)を設定します。
	/// </summary>
	D3D12_BLEND_DESC SetBlendStateAdd();

	/// <summary>
	/// ブレンドステート(BlendMode::Subtract用)を設定します。
	/// </summary>
	D3D12_BLEND_DESC SetBlendStateSubtract();

	/// <summary>
	/// ブレンドステート(BlendMode::Multiply用)を設定します。
	/// </summary>
	D3D12_BLEND_DESC SetBlendStateMultiply();

	/// <summary>
	/// ブレンドステート(BlendMode::Screen用)を設定します。
	/// </summary>
	D3D12_BLEND_DESC SetBlendStateScreen();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr uint32_t kInputElementCount = 3;		/* 入力要素数 */
	static constexpr uint32_t kDescriptorRangeCount = 1;	/* ディスクリプタレンジ数 */
	static constexpr uint32_t kRootParameterCount = 5;		/* ルートパラメーター数 */
	static constexpr uint32_t kStaticSamplerCount = 1;		/* サンプラー数 */

	static constexpr uint32_t kDescriptorRangeBaseShaderRegister = 0; /* ディスクリプタレンジの基底レジスタ */
	static constexpr uint32_t kDescriptorRangeNumDescriptors = 1;	  /* ディスクリプタレンジの数 */

	static constexpr uint32_t kRootParameterIndexMaterial = 0;	/* マテリアル用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexTransform = 1;	/* 変換行列用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexTexture = 2;	/* テクスチャ用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexLight = 3;		/* ライト用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexCamera = 4;	/* カメラ用ルートパラメーターインデックス */

	static constexpr uint32_t kMaterialShaderRegister = 0;	/* マテリアルのシェーダーレジスタ番号 */
	static constexpr uint32_t kTransformShaderRegister = 0;	/* 変換行列のシェーダーレジスタ番号 */
	static constexpr uint32_t kLightShaderRegister = 1;		/* ライトのシェーダーレジスタ番号 */
	static constexpr uint32_t kCameraShaderRegister = 2;	/* カメラのシェーダーレジスタ番号 */

	static constexpr uint32_t kSamplerShaderRegister = 0;	/* サンプラーのシェーダーレジスタ番号 */

	static constexpr uint32_t kRenderTargetCount = 1; /* レンダーターゲット数 */
	static constexpr uint32_t kRenderTargetIndex = 0; /* レンダーターゲットインデックス */

	static constexpr uint32_t kSampleDescCount = 1;	/* サンプル数 */

	static constexpr uint32_t kInputElementIndexPositon = 0;	/* POSITION入力要素インデックス */
	static constexpr uint32_t kInputElementIndexTexcoord = 1;	/* TEXCOORD入力要素インデックス */
	static constexpr uint32_t kInputElementIndexNormal = 2;		/* NORMAL入力要素インデックス */
	static constexpr uint32_t kSemanticIndex = 0;				/* セマンティックインデックス */

	static constexpr uint32_t kDSVDescriptorCount = 1;	/* DSVディスクリプタヒープ数 */
	static constexpr uint32_t kDSVHeapIndex = 0;		/* DSVヒープインデックス */
	static constexpr uint32_t kDescriptorHeapCount = 1;	/* ディスクリプタヒープ数 */

	static constexpr uint32_t kRootSignatureVersion = 0; /* ルートシグネチャバージョン */

	// =========================================================
	// Member Variables
	// =========================================================

	// ----- System -----
	DirectXBase* dxBase_;																	/* DirectX基盤クラス */

	// ----- Resources -----
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;								/* ルートシグネチャ */
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;						/* グラフィックスパイプラインステート */

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;										/* ルートシグネチャシグネチャバッファ */
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;											/* エラーバッファ */

	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[kInputElementCount];						/* 入力レイアウト */
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;												/* 入力レイアウト構造体 */

	IDxcUtils* dxcUtils_;																	/* DXCユーティリティ */
	IDxcCompiler3* dxcCompiler_;															/* DXCコンパイラ */
	IDxcIncludeHandler* includeHandler_;													/* インクルードハンドラ */
	IDxcBlob* vertexShaderBlob_;															/* 頂点シェーダバイトコード */
	IDxcBlob* pixelShaderBlob_;																/* ピクセルシェーダバイトコード */

	D3D12_RASTERIZER_DESC rasterizerDesc_;													/* ラスタライザステート */

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;							/* 深度バッファ */
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_;												/* 深度ステンシルステート */
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_;													/* 深度ステンシルビュー */
	DescriptorHeap dsvDescriptorHeap_;														/* 深度ステンシルビュー用デスクリプタヒープ */

	// ----- BlendState -----
	D3D12_BLEND_DESC blendDesc_;															/* ブレンドステート */
	D3D12_BLEND_DESC blendDescNone_;														/* ブレンドステート(BlendMode::None用) */
	D3D12_BLEND_DESC blendDescAdd_;															/* ブレンドステート(BlendMode::Add用) */
	D3D12_BLEND_DESC blendDescSubtract_;													/* ブレンドステート(BlendMode::Subtract用) */
	D3D12_BLEND_DESC blendDescMultiply_;													/* ブレンドステート(BlendMode::Multiply用) */
	D3D12_BLEND_DESC blendDescScreen_;														/* ブレンドステート(BlendMode::Screen用) */

	// ----- PSO -----
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBlendModeNone_;		/* グラフィックスパイプラインステート(BlendMode::None用) */
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBlendModeAdd_;			/* グラフィックスパイプラインステート(BlendMode::Add用) */
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBlendModeSubtract_;	/* グラフィックスパイプラインステート(BlendMode::Subtract用) */
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBlendModeMultiply_;	/* グラフィックスパイプラインステート(BlendMode::Multiply用) */
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBlendModeScreen_;		/* グラフィックスパイプラインステート(BlendMode::Screen用) */
};
}
