#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <DescriptorHeap.h>
#include <DirectXBase.h>

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
	// Member Variables
	// =========================================================

	// ----- System -----
	DirectXBase* dxBase_;																	/* DirectX基盤クラス */

	// ----- Resources -----
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;								/* ルートシグネチャ */
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;						/* グラフィックスパイプラインステート */

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;										/* ルートシグネチャシグネチャバッファ */
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;											/* エラーバッファ */

	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3];											/* 入力レイアウト */
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
